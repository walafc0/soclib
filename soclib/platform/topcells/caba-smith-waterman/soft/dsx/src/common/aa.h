short char2AA(char ch)
{
        switch (ch) {
                case 'C' : return 0;
                case 'G' : return 1;
                case 'P' : return 2;
                case 'S' : return 3;
                case 'A' : return 4;
                case 'T' : return 5;
                case 'D' : return 6;
                case 'E' : return 7;
                case 'N' : return 8;
                case 'Q' : return 9;
                case 'H' : return 10;
                case 'K' : return 11;
                case 'R' : return 12;
                case 'V' : return 13;
                case 'M' : return 14;
                case 'I' : return 15;
                case 'L' : return 16;
                case 'F' : return 17;
                case 'Y' : return 18;
                case 'W' : return 19;
                default  :
			return 0;
                }
}


char AA2char(short x){
        switch (x) {
                case 0 : return 'C';
                case 1 : return 'G';
                case 2 : return 'P';
                case 3 : return 'S';
                case 4 : return 'A';
                case 5 : return 'T';
                case 6 : return 'D';
                case 7 : return 'E';
                case 8 : return 'N';
                case 9 : return 'Q';
                case 10: return 'H';
                case 11: return 'K';
                case 12: return 'R';
                case 13: return 'V';
                case 14: return 'M';
                case 15: return 'I';
                case 16: return 'L';
                case 17: return 'F';
                case 18: return 'Y';
                case 19: return 'W';
                default  : 
			return 'C';
                }
}

