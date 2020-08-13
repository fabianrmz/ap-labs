def getLength(element):
    print(setCycle(element,0))
    
def setCycle(element,n):
    if (isinstance(element,int)):
        return n+1
    elif (isinstance(element,list)):
        for x in element:  
            n=int(setCycle(x,n))
    return int(n);

getLength([1, [2, 3]])
getLength([1, [2, [3, 4]]])
getLength([1, [2, [3, [4, [5, 6]]]]])
getLength([1, [2], 1, [2], 1])