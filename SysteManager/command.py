def commands(command, manager):

    posAddr = command.find(';')
    inputAddr = command[:posAddr] if posAddr != -1 else ''
    inputData = command[posAddr+1:] if posAddr != -1 else command
    posCmd = inputData.find('=')
    inputElem = inputData[:posCmd] if posCmd != -1 else inputData
    inputParam = inputData[posCmd+1:] if posCmd != -1 else ''
    inputArray = inputElem.split('\\')
    # 对输入数据进行划分

    cur = manager

    if inputElem == '': return cur
    #空格则直接返回脚本的内容


    for i in range(len(inputArray)):
        #遍历每一层

        try:

            obj = cur[inputArray[i]]


            if type(obj) == str and i == len(inputArray)-1 and inputParam:
                cur[inputArray[i]] = inputParam
            else:
                cur = obj


        except KeyError as e:
            cur = ''
            break
    
    if inputParam == '':
        return str(cur)
    else:
        return ''