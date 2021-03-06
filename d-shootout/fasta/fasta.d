import std.stdio, std.conv;

class Random {
    private:
        float seed = 42;
        const int IA = 3877, IC = 29573, IM = 139968;

    public:
        final float genRandom(float max) {
            return max * ((seed = (seed * IA + IC) % IM) * (1.0 / IM));
        }
}


class Fasta: Random {
    public:
        enum TableType {
            iub,
            homosapiens
        }

        final void makeRepeatFasta(string id, string desc, int n) {
            const int length = 60;
            int k = 0;

            stdout.writefln(">", id, " ", desc);
            char[length] line;
            while (n > 0) {
                int m = n < length ? n : length;
                for (int j = 0; j < m; j++, k++) {
                    if (k >= alu.length)
                        k = 0;
                    line[j] = alu[k];
                }
                stdout.writeln(line[0 .. m]);
                n -= length;
            }
        }

        final void makeRandomFasta(string id, string desc, TableType tableType, int n) {
            const int length = 60;
            IUB[] table;

            switch (tableType) {
                case TableType.iub:
                    table = iub;
                    break;
                default:
                    table = homosapiens;
                    break;
            }

            stdout.writefln(">", id, " ", desc);
            makeCumulative(table);
            char[length] line;
            while (n > 0) {
                int m = n < length ? n : length;
                for (int j; j < m; j++) {
                    float rval = genRandom(1);
                    for (int i; i < table.length; i++) {
                        if (rval < table[i].p) {
                            line[j] = table[i].c;
                            break;
                        }
                    }
                }
                stdout.writeln(line[0 .. m]);
                n -= length;
            }
        }


    private:
        struct IUB {
            float p;
            char c;
        }

        IUB[] iub = [IUB(0.27, 'a'),
                     IUB(0.12, 'c'),
                     IUB(0.12, 'g'),
                     IUB(0.27, 't'),
                     IUB(0.02, 'B'),
                     IUB(0.02, 'D'),
                     IUB(0.02, 'H'),
                     IUB(0.02, 'K'),
                     IUB(0.02, 'M'),
                     IUB(0.02, 'N'),
                     IUB(0.02, 'R'),
                     IUB(0.02, 'S'),
                     IUB(0.02, 'V'),
                     IUB(0.02, 'W'),
                     IUB(0.02, 'Y')];

        IUB[] homosapiens = [IUB(0.3029549426680, 'a'),
                             IUB(0.1979883004921, 'c'),
                             IUB(0.1975473066391, 'g'),
                             IUB(0.3015094502008, 't')];

        const string alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" ~
                           "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" ~
                           "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" ~
                           "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" ~
                           "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" ~
                           "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" ~
                           "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

        void makeCumulative(IUB[] table) {
            float prob = 0.0;
            foreach(ref iub; table) {
                prob += iub.p;
                iub.p = prob;
            }
        }
}


void main(string[] args) {
    int n = args.length == 2 ? to!int(args[1]) : 1;

    scope Fasta fasta = new Fasta;

    fasta.makeRepeatFasta("ONE", "Homo sapiens alu", n*2);
    fasta.makeRandomFasta("TWO", "IUB ambiguity codes", Fasta.TableType.iub, n*3);
    fasta.makeRandomFasta("THREE", "Homo sapiens frequency", Fasta.TableType.homosapiens, n*5);
}
