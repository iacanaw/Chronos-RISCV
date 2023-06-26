
def getState(a,b,c):
    return ((a*a*a - 18*a*a + 107*a) / 6) + ((11*b - b*b - 2*a*b) / 2) + c

    # state_x = (int)(x ? ((x*x*x - 18*x*x + 107*x) / 6) : 0);
    # state_y = (int)(y ? ((11*y - y*y - 2*x*y) / 2) : 0);
    # state_xyz = state_x + state_y + z;

def getState2(a,b,c,d,e,f):
    i = 0
    for a0 in range(5):
        for b0 in range(5):
            for c0 in range(5):
                for ad in range(5):
                    for bd in range(5):
                        for cd in range(5):
                            if a0 == a and b0 == b and c0 == c and ad == d and bd == e and cd == f:
                                return i
                            elif (a0+b0+c0) <= 4 and  (ad+bd+cd)<=4:
                                i+=1       
            

for a in range(5):
    for b in range(5):
        for c in range(5):
             for ad in range(5):
                 for bd in range(5):
                     for cd in range(5):
                        if(a+b+c)<=4 and (ad+bd+cd)<=4:
                            print(str(a)+" "+str(b)+" "+str(c)+" "+str(ad)+" "+str(bd)+" "+str(cd)+" "+str(int(getState2(a,b,c,ad,bd,cd))))