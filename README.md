# Matrix pack-recorder

## 介绍

pack-recorder 主要来帮助客户录制raw数据。
使用方法，参考如下步骤：（完整代码参见pack-recorder/src/main.cpp文件）
1. 初始化
    ```cpp
      /* set Matrix's ip address. */
      const char *matrix_ip = "192.168.1.10";
      NetworkReceiverIntf *network_receiver = new NetworkReceiverIntf();
      /* initialize connection to Matrix */
      network_receiver->Init(matrix_ip);
      network_receiver->PackFileInit("ADAS_test.pack");  // 此处定义录制pack文件名称
    ```

2. 初始化UDP log，此处log为Matrix侧接收的UDP log，此处UDP log不影响录制，可关闭
    ```cpp
      /* receive Matrix's log via UDP */
      network_receiver->UDPLogReceiverInit();
      network_receiver->UDPLogAddTargetServer(matrix_ip);
      std::thread udp_log_recv([network_receiver] {
        std::string log;
        while (g_log_recv) {
          if (network_receiver->UDPLogRecv(log) == 0) {
            /* you can deal with the Matrix log here */
            printf("%s", log.c_str());
          }
        }
      });
    ```

3. 打开by pass录制接口，设定为录制by pass模式
    ```cpp
    // set by pass mode.
    network_receiver->SetPackRecordMode(PackRecordMode::ByPass);
    ```

4. 按照如下格式，填充图像采集设备的图像数据
    ```cpp
    void RecvByPassMockFunc(NetworkReceiverIntf * network_receiver) {
      // endless loop, read raw image data from other resource
      while(true) {
        BypassImageMessage bypassImage;
        // fake bypass data.
        bypassImage.channel_ = 3;      // Raw image channel is 3
        bypassImage.counter_ = 0;      // Current should set to 0
        bypassImage.format_ = 15;      // ImageFormat::RAW12 = 15, refer
                                      // common.proto from matrix-sample-code
        bypassImage.frame_id_ = -1;    // Frame id
        bypassImage.height_ = 1080;    // Image height
        bypassImage.width_ = 2048;     // Image width
        bypassImage.time_stamp_ = fake_time_stamp;  // Timestamp of by pass image
        /* Fake image data from file. */
        std::string test_raw_file = "RCCBRaw12.data";
        std::ifstream ifs(test_raw_file, std::ios::binary);
        if (!ifs.is_open()) {
          std::cout << "Please set the correct raw data path." << std::endl;
          return ;
        }

        // Read image into buf
        ifs.seekg(0, std::ios::end);
        int file_len = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        bypassImage.raw_img_.resize(file_len);
        ifs.read(reinterpret_cast<char *>(bypassImage.raw_img_.data()), file_len);
        /*  Push by pass image data to a queue. */
        network_receiver->SaveByPassImage(&bypassImage);
        std::cout << "Save bypass image data, time_stamp: " << fake_time_stamp
          << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        fake_time_stamp += 33;
      }
    }
    ```

5. 保存收取到的raw图信息到缓存队列（存储线程）
    启动存储by pass raw image线程
    ```cpp

    std::thread by_pass_image_recv(RecvByPassMockFunc, network_receiver);
    ```

6. 接收Matrix设备发送的网络数据帧,并录制pack（主线程）
    ```cpp
    while (true) {
      ...
      ret = network_receiver->RecvFrame(&frame_info); // 接收网络帧
      network_receiver->WriteFrameToPackFile(&frame_info); // 录制pack
      ...
    }
    ```

7. 使用客户端可播放录制的pack文件`ADAS_test.pack`

8. 注意：录制数据时，Matrix设备（提供感知数据网络帧）需要设定为无图模式：即设定板载配置文件
`/mnt/adas-rt/config/engine.json`的`send_mode`为`4`


## 编译
目前仅支持windows平台下release版本。

### windows
要求：
- windows
- visual studio 2015
- cmake (>=3.0)

windows 平台目前支持的编译工具只有msvc， x86 和 x86_64 architecture

#### x86
1. 进入工程目录，新建 build 目录； `cd pack-recorder `,` mkdir -p build/win32`
2. 进入build/win32目录；`cd build/win32`
3. `cmake ../.. -G "Visual Studio 14 2015" `

#### x86_64
1. 进入工程目录，新建 build 目录； `cd pack-recorder `,` mkdir -p build/win64`
2. 进入build/win64目录；`cd build/win64`
3. `cmake ../.. -G "Visual Studio 14 2015 Win64" `
