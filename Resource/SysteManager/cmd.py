def commands(manager):

    
    if manager['Configure']['Program Mode'] == 'server':
        # 服务器模式
        while True:
            pass

    elif manager['Configure']['Program Mode'] == 'usually':
        # 通常模式
        command = ''
        while manager['status'] != 'shutdown':
            print('\n->',end='')
            command = input()
            #if command == 'quit': break
            # 循环和出口

            inputData = command.split('=') 
            inputArray = inputData[0].split('\\')
            inputParam = inputData[len(inputData)-1] if len(inputData) > 1 else ''
            # 对输入数据进行划分

            cur = manager
            for i in range(len(inputArray)):
                #遍历每一层

                try:
                    obj = cur[inputArray[i]]
                    # 尝试进入下一层

                    if i == len(inputArray)-1 and type(obj) == str and inputParam:
                        #当输入参数不为空且当前层为字符串时
                        cur[inputArray[i]] = inputParam #为当前层赋值
                    else:
                        # 尝试进入下一层
                        cur = obj

                except Exception as e:
                    break
    
            if inputParam == '':
                manager['printer'].justPrint(str(cur))