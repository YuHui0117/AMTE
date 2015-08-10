#!/usr/local/bin/python2.7 
import sys

fin1 = open(sys.argv[1]);
fin2 = open(sys.argv[2]);
fin3 = open(sys.argv[3]);
arg = float(sys.argv[4])
line1 = fin1.readline()
line2 = fin2.readline()
line3 = fin3.readline()
dict1={}
while line1:
    strlistd = line1.split('\t')
    if(len(strlistd)==6):
        newstr =  strlistd[1] + '\t' + strlistd[2] + '\t' + strlistd[3] + '\t'+strlistd[4]
        dict1[newstr] = float(strlistd[5])
    line1 = fin1.readline()

dict2={}
while line2:
    strlistm = line2.split('\t')
    newstr =  strlistm[1] + '\t' + strlistm[2] + '\t' + strlistm[3] +'\t'+strlistm[4]
    dict2[newstr] = float(strlistm[5])
    line2 = fin2.readline()

dict3={}
while line3:
    strlistm = line3.split('\t')
    newstr =  strlistm[1] + '\t' + strlistm[2] + '\t' + strlistm[3] +'\t'+strlistm[4]
    dict3[newstr] = float(strlistm[5])
    line3 = fin3.readline()

for key,value in dict1.iteritems():
    val1 = value
    if(key in dict2):
        val2 = dict2[key]
        if(key in dict3):
            val3 = dict3[key]
            if(val2==0 or val3==0):
                score = 0
            else:
                score = val1 * 1.0/(arg/val3 + (1.0-arg)/val2 )
            if (score < 0):
                score = 0
            newstr = 'comb'+'\t'+ key + '\t'+ str(score)
            print newstr

