#node 1
MCK=42000000
CAN_baudrate = 125000
bit_time = 1/CAN_baudrate
F_CPU = 84000000

t_q = 16
tcsc = bit_time/t_q
brp = (tcsc*F_CPU) -1

#velger sampling point 87,5%-> 14 tq
SJW = 1 #setter denne til å være 1 (sync seg)
#propag + phase1 = 14-SJW=13 (2 tq til phase2)


PROPAG = 7 #mellom 1-8
PHASE1 = 6 #mellom 1-8

PHASE2= t_q - (PROPAG+PHASE1+SJW) #må være over 0

SMP = 0 #sampler 1 gang
print("-----NODE2----")
print("brp: ", brp)
print("PROPAG: ", PROPAG)
print("PHASE1: ", PHASE1)
print("PHASE2: ", PHASE2)
print("SJW: ", SJW)

#node 2
FOSC_MCK =16000000
brp_node1= (tcsc * FOSC_MCK) /2 -1

print("-----NODE1----")
print("brp: ", brp_node1)

