import std.stdio, std.string;
import std.conv : to;
import core.stdc.stdio : putc;

void main(string[] args)
{
  char bit_num = 0, byte_acc = 0;
  int n = args.length > 1 ? to!int(args[1]) : 1;
  const iter = 50;
  const lim = 2.0 * 2.0;

  writefln("P4\n%d %d",n,n);

  for(auto y=0; y<n; y++)
  {
    for(auto x=0; x<n; x++)
    {
       auto Z = 0 + 0i; auto C = 2*cast(double)x/n - 1.5 + 2i*cast(double)y/n - 1i;

       for(auto i = 0; i < iter && norm(Z) <= lim; i++)
         Z = Z*Z + C;

       byte_acc = cast(byte) (byte_acc << 1) | ((norm(Z) > lim) ? 0x00:0x01);

       bit_num++;
       if(bit_num == 8)
       {
           stdout.write(byte_acc);
           bit_num = byte_acc = 0;
       }
       else if(x == n-1)
       {
           byte_acc  <<= (8-n%8);
           stdout.write(byte_acc);
           bit_num = byte_acc = 0;
       }
    }
  }
}

double norm(cdouble C)
{
    return C.re*C.re + C.im*C.im;
}
