import std.stdio, std.string, std.algorithm;

int[string] gen_freq(string seq, size_t frame) {
    int[string] freqs;
    auto ns = seq.length + 1 - frame;

    for (auto ii = 0; ii < ns; ++ii)
        freqs[ seq[ii .. ii + frame] ]++;
    return freqs;
}

void sort_seq(string seq, int length) {
    auto n = seq.length + 1 - length;
    auto freqs = gen_freq(seq, length);

    struct Pair {
        string k;
        int v;
        int opCmp(Pair otherPair) {
            return (v > otherPair.v) ? -1 : 1;
        }
    }

    auto pairs = new Pair[freqs.length];
    uint i = 0;
    foreach(k, v; freqs) {
        pairs[i] = Pair(k, v);
        i++;
    }

    foreach(p; sort(pairs))
        writeln("%s %.3f", p.k, 100.0*p.v/n);
    writeln();
}

void find_seq(string seq, string s) {
    auto t = gen_freq(seq, s.length);
    writeln((s in t) ? t[s] : 0, '\t', s);
}

void main() {
    foreach(line; stdin.byLine)
        if (line[0 .. 3] == ">TH")
            break;

    string[] seq;
    foreach(line; stdin.byLine) {
        if ((line[0] == '>') || (line[0] == ';'))
            break;
        seq ~= line.dup.chomp();
    }

    auto sequence = seq.join("").toUpper();

    sort_seq(sequence, 1);
    sort_seq(sequence, 2);

    foreach(se; split("GGT GGTA GGTATT GGTATTTTAATT GGTATTTTAATTTATAGT"))
        find_seq(sequence, se);
}
