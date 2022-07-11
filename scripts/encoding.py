
def getState(x, y, z, a, b, c):
    state = 0

    for ai in range(5):
        for bi in range(5):
            for ci in range(5):
                if ai+bi+ci <= 4:
                    for xi in range(5):
                        for yi in range(5):
                            for zi in range(5):
                                if xi+yi+zi <= 4:
                                    if ai == a and bi == b and ci == c and xi == x and yi == y and zi == z:
                                        return state
                                    state+=1
    return 0


def getState2(x, y, z, a, b, c):
    state_x = (int)(0 if x == 0 else (x**3 - 18*x**2 + 107*x) / 6)
    state_y = (int)(0 if y == 0 else (11*y - y**2 - 2*x*y)/2)
    state_xyz = state_x + state_y + z

    state_a = (int)(0 if a == 0 else (a**3 - 18*a**2 + 107*a) / 6)
    state_b = (int)(0 if b == 0 else (11*b - b**2 - 2*a*b)/2)
    state_abc = state_a + state_b + c

    state = state_abc*35 + state_xyz

    return state


print("Possible States: ")
i = 0
erro_1 = 0
erro_2 = 0
for a in range(5):
    for b in range(5):
        for c in range(5):
            if a+b+c <= 4:
                for x in range(5):
                    for y in range(5):
                        for z in range(5):
                            if x+y+z <= 4:
                                if i != getState2(x,y,z,a,b,c):
                                    print("State "+str(i)+": ("+str(x)+", "+ str(y)+", "+str(z)+", "+str(a)+", "+ str(b)+", "+str(c)+") => getState = "+str(getState2(x,y,z,a,b,c))+" XXXXX")
                                    erro_2 += 1
                                else:
                                    print("State "+str(i)+": ("+str(x)+", "+ str(y)+", "+str(z)+", "+str(a)+", "+ str(b)+", "+str(c)+") => getState = "+str(getState2(x,y,z,a,b,c)))                               
                                i+=1

print("Errors: "+str(erro_2))
 