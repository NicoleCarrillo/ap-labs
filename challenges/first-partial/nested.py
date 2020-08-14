#wanted to know the total number of non-nested items in the nested list. 
#In the above case, [1, [2, 3]] contains 3 non-nested items, 1, 2 and 3.

def get_length(lista):
    contador = 0
    for x in lista:
        if type(x) == list:  
            contador += get_length(x)
        else:
            contador += 1    
    return contador

lista=[];
contador = get_length(lista)
print(contador)