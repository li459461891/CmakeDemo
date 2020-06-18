|阶段|log示例|说明|
|:-|:-|:-|
|接收接口返回|recv msg from MCU, len: $1, dev type: $2|$1: 接收数据返回值，小于0：接收失败错误码，大于0：接收到的数据长度， 等于0：但是MCU发送数据大于0也是接收失败; $2: dev_type(0-3)，数据小于0即接收错误|
|接收到数据|received MCU data, whole length：1, data length: $2, Type:$3|$1:总长度；<br/>$2: data的长度；<br/>$3:数据类型，$3==2表示接收到CAN数据
