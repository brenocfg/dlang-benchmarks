#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <sched.h>
#include <memory.h>

// need "-fopenmp" flag when compile
#include <omp.h>

#define L2_CACHE_LINE   64
#define BYTE_A_TIME      L2_CACHE_LINE
#define COLUMN_FETCH    (BYTE_A_TIME * 8)


typedef double   v2d   __attribute__ ((vector_size(16)));

const v2d v10   = { 1.0, 1.0 };
const v2d v15   = { 1.5, 1.5 };
const v2d v40   = { 4.0, 4.0 };

v2d inv_2n;
v2d inv_4n;


int 
GetThreadCount()
{
   cpu_set_t cs;
   CPU_ZERO(&cs);
   sched_getaffinity(0, sizeof(cs), &cs);

   int count = 0;
   for (int i = 0; i < CPU_SETSIZE; ++i)
   {
      if (CPU_ISSET(i, &cs))
         ++count;
   }
   return count;
}


struct MB_Element
{
private:
   v2d   Crv, Civ, Zrv, Ziv, Trv, Tiv;
   
public:
   int result;
   
   MB_Element(int r, v2d cimg)
   {
      double tmp[2] = {r+1, r};
      Crv = __builtin_ia32_loadupd(tmp);
      
      Crv = Crv * inv_2n - v15;
      Civ = cimg;

      Zrv = Crv;
      Ziv = cimg;

      Trv = Crv * Crv;
      Tiv = cimg * cimg;

      result = 3;
   }

   MB_Element(MB_Element const& o)
   {
      Crv = o.Crv + inv_4n;
      Civ = o.Civ;
      
      Zrv = Crv;
      Ziv = o.Ziv;
      
      Trv = Crv * Crv;
      Tiv = o.Tiv;
      
      result = 3;
   }

   int
   eval()
   {
      v2d ZZ = Zrv * Ziv;
      Zrv = Trv - Tiv + Crv;
      Ziv = ZZ + ZZ + Civ;
      Trv = Zrv * Zrv;
      Tiv = Ziv * Ziv;

      v2d delta = (v2d)__builtin_ia32_cmplepd( (Trv + Tiv), v40 );
      result &= __builtin_ia32_movmskpd(delta);

      return result;
   }
};

void 
mandelbrot(int N, char* data)
{
   __attribute__ ((aligned(L2_CACHE_LINE))) int jobs[N];
   memset(jobs, 0, sizeof(jobs));

	#pragma omp parallel default(shared) firstprivate(data) num_threads(GetThreadCount())
	{
      for (int y = 0; y < N; ++y, data += (N >> 3)) 
      {
         v2d Civ = {y, y};
         Civ = Civ * inv_2n - v10;

         int x;
         while ((x = __sync_fetch_and_add(jobs + y, COLUMN_FETCH)) < N)
         {
            int limit = std::min(x +COLUMN_FETCH, N);
            for (; x < limit; x += 8)
            {
               MB_Element   e1(x, Civ), e2(e1), e3(e2), e4(e3);
               
               int i = 1;
               while ( (e1.result || e2.result || e3.result || e4.result) 
                     && (i++ < 50) )
               {
                  e1.eval();
                  e2.eval();
                  e3.eval();
                  e4.eval();
               }   
               
               int byte_acc = (e1.result << 6)|(e2.result << 4)|(e3.result << 2)|e4.result;
               data[x >> 3] = static_cast<char>(byte_acc);
            }
         }
      }
   }
}


int 
main (int argc, char **argv)
{
   int N = (argc == 2) ? atoi(argv[1]) : 200;
   assert((N % 8) == 0);

   printf("P4\n%d %d\n", N, N);
   int width_bytes = N >> 3;

   {
      double t[2];
      t[0] = t[1] = 2.0 / N;
      inv_2n = __builtin_ia32_loadupd(t);
      inv_4n = inv_2n + inv_2n;
   }

   char* data = 0;
   assert(   posix_memalign(reinterpret_cast<void**>(&data), L2_CACHE_LINE, width_bytes * N)
         == 0);

   mandelbrot(N, data);

   fwrite( data, width_bytes, N, stdout);
   free(data);

   return 0;
}

