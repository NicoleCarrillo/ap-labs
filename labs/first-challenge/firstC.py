def elementos(lt):
    num = 0
    for x in lt:
        if type(x) == list:  
            num += elementos(x)
        else:
            num += 1    
    return num

lt=[1, [1, [1, [1]]],1]
num = elementos(lt)
print(num)