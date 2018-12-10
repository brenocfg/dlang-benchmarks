#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include <sched.h>
#include <pthread.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>

typedef unsigned long long u64;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned char u8;

using namespace std;

struct CPUs {
   CPUs() {
      cpu_set_t cs;
      CPU_ZERO( &cs );
      sched_getaffinity( 0, sizeof(cs), &cs );
      count = 0;
      for ( size_t i = 0; i < CPU_SETSIZE; i++ )
         count += CPU_ISSET( i, &cs ) ? 1 : 0;
      count = std::max( count, u32(1) );
   }

   u32 count;
} cpus;

template< u32 N > struct TypeSelector;
template<> struct TypeSelector<4> { enum { bits = 32, }; typedef u32 tint; };
template<> struct TypeSelector<8> { enum { bits = 64, }; typedef u64 tint; };

template< u32 N > struct PreferU64 { 
   enum { bits = TypeSelector<8>::bits }; 
   typedef typename TypeSelector<8>::tint tint;
};

template<> struct PreferU64<4> {
   enum { selector = sizeof(u32 *) };
   enum { bits = TypeSelector<selector>::bits }; 
   typedef TypeSelector<selector>::tint tint;
};

template< u32 N >
struct DNASource {
   enum {
      completedwords = N / 4,
      partialbytes = N & 3,
      storagedwords = ( N + 15 ) / 16,
      storagebytes = storagedwords * 4,

      bits = PreferU64<storagebytes>::bits,
      bytes = bits / 8,
      maxsequences = bits / 2,
      sequencebits = N * 2,
   };
   typedef typename TypeSelector<storagebytes>::tint tint;
   typedef typename PreferU64<storagebytes>::tint tstore;

   DNASource( const char *data, const u32 offset ) : in(data) {
      const u32 partial = offset & ( maxsequences - 1 );
      const u32 rshift = partial * 2, lshift = bits - rshift;
      in += ( offset / maxsequences );
      pack(0); pack(1);
      state = ( partial ) ? ( lower[0] >> rshift ) | ( lower[1] << lshift ) : lower[0];
      left0 = maxsequences;
      left1 = lshift / 2;
   }

   void pack( const u32 slot ) {
      u8 *out = (u8 *)&lower[slot];

      for ( u32 i = 0; i < bytes; i++, in += 4 ) {
         u32 conv = ( *(const u32 *)in >> 1 ) & 0x03030303;
         *out++ = conv | ( conv >> 6 ) | ( conv >> 12 ) | ( conv >> 18 );
      }
   }

   inline void getandadvance( tint &out, const u32 increment = N ) {
      if ( ( N > maxsequences / 2 ) || ( left0 < N ) ) {
         u32 want = maxsequences - left0;
         state |= ( lower[1] >> ( ( maxsequences - left1 ) * 2 ) ) << ( left0 * 2 );
         if ( left1 > want ) {
            left1 -= want;
         } else {
            lower[0] = lower[1];
            left1 += left0;
            pack(1);
         }
         if ( left1 != maxsequences )
            state |= ( lower[1] << ( left1 * 2 ) );
         left0 = maxsequences;
      }

      if ( sequencebits != bits ) {
         tstore shift = sequencebits, mask = ( tstore(1) << shift ) - 1;
         out = tint(state & mask);
      } else {
         out = tint(state);
      }
      state >>= ( increment * 2 );
      left0 -= increment;
   }

protected:
   const char *in;
   u32 left0, left1;
   tstore state, lower[2];
};

template< u32 N >
struct Key {
   typedef typename DNASource<N>::tint tint;

   struct Ops {
      u32 operator() ( const Key &k ) const {
         if ( N <= 4 ) {
            return u32(~k);
         } else if ( N <= 16 ) {
            u8 shift = N / 2;
            return u32(~k + ( ~k >> shift ));
         } else {
            u8 shift = N / 2;
            return u32(~k + ( ~k >> 13 ) + ( ~k >> shift ));
         }
      }

      bool operator() ( const Key &a, const Key &b ) const { return ~a == ~b; }
   };

   Key() {}

   Key( const char *in ) : packed(0) {
      u8 *bytes = (u8 *)&packed;
      for ( u32 i = 0; i < N; i++ )
         bytes[i/4] |= ( ( *in++ >> 1 ) & 0x3 ) << ( ( i % 4 ) * 2 );
   }

   const char *tostring() const {
      static char names[2][N+1], table[4] = { 'A', 'C', 'T', 'G' };
      static u32 on = 0;
      u64 bits = packed;
      on ^= 1;
      for ( u32 i = 0; i < N; i++, bits >>= 2 )
         names[on][i] = table[bits & 3];
      names[on][N] = 0;
      return names[on];
   }

   bool operator< ( const Key &b ) const {
      return strcmp( tostring(), b.tostring() ) < 0;
   }

   tint &operator~ () { return packed; }
   const tint &operator~ () const { return packed; }

protected:
   tint packed;
};

template< u32 N >
   class KeyHash :
      public __gnu_pbds::cc_hash_table <
         Key<N>,
         u32,
         typename Key<N>::Ops,
         typename Key<N>::Ops
      > {};

static const u32 lengths[] = { 1, 2, 3, 4, 6, 12, 18 }, numLengths = 7;
static const u32 lineLength = 60;

static const u32 sequentialMax = 8;

struct Block {
   Block() : data(NULL), count(0), alloc(4 * 1048576) {
      data = (char *)realloc( data, alloc );
   }

   ~Block() { free( data ); }

   void read() {
      data[lineLength] = -1;
      while ( fgets_unlocked( data + count, lineLength + 2, stdin ) ) {
         if ( data[count] == '>' )
            return;
         
         if ( data[count + lineLength] != 0xa ) {
            count += u32(strlen( data + count )) - 1;
            data = (char *)realloc( data, count + 64 * 2 );
            return;
         }

         count += lineLength;
         if ( ( ( count + lineLength ) ) > alloc ) {
            alloc *= 2;
            data = (char *)realloc( data, alloc );
         }

         data[count + lineLength] = -1;
      }
   }

   bool untilheader( const char *match ) {
      size_t len = strlen( match );
      const u32 *in = (const u32 *)data, *want = (const u32 *)match;
      while ( fgets_unlocked( data, alloc, stdin ) )
         if ( ( *in == *want ) && ( memcmp( data, match, len ) == 0 ) )
            return true;
      return false;
   }

   u32 getcount() const { return count; }
   char *getdata() { return data; }

protected:
   char *data;
   u32 count, alloc;
};

struct Queue {
   Queue() : st(0) {}

   bool get( u32 &sequence, u32 &offset ) {
      while ( true ) {
         u32 cur = st;
         if ( ( cur >> 16 ) == numLengths )
            return false;

         if ( __sync_val_compare_and_swap( &st, cur, nextstate( cur ) ) != cur )
            continue;

         // it's ours
         sequence = lengths[cur >> 16];
         offset = cur & 0xffff;
         return true;
      }
   }

   u32 nextstate( u32 cur ) {
      u16 offset = ( cur & 0xffff ), length = ( cur >> 16 );
      if ( ( lengths[length] <= sequentialMax ) || ( ++offset == lengths[length] ) ) {
         offset = 0;
         length++;
      }
      return ( length << 16 ) | offset;
   }

protected:
   volatile u32 st;
};


struct Worker {
   Worker() {}

   template< u32 N, class Hash >
   void process( Hash &hash ) {
      Key<N> key;
      DNASource<N> source( block->getdata(), offset );
      const u32 advance = ( N <= sequentialMax ) ? 1 : N;
      for ( u32 i = block->getcount() - offset; i >= N; i -= advance ) {
         source.getandadvance( ~key, advance );
         hash[key]++;
      }
   }

   void run() {
      while ( workQueue->get( length, offset ) ) {
         switch ( length ) {
            case 1: process<1>( hash1 ); break;
            case 2: process<2>( hash2 ); break;
            case 3: process<3>( hash3 ); break;
            case 4: process<4>( hash4 ); break;
            case 6: process<6>( hash6 ); break;
            case 12: process<12>( hash12 ); break;
            case 18: process<18>( hash18 ); break;
            default: break;
         }
      }
   }

   void join() { pthread_join( handle, 0 ); }
   void start( Queue *queue, Block *in ) {
      workQueue = queue;
      block = in;
      pthread_create( &handle, 0, Worker::thread, this );
   }
   static void *thread( void *arg ) { ((Worker *)arg)->run(); return 0; }

   pthread_t handle;
   Block *block;
   Queue *workQueue;
   u32 length, offset;

   KeyHash<18> hash18;
   KeyHash<12> hash12;
   KeyHash<6> hash6;
   KeyHash<4> hash4;
   KeyHash<3> hash3;
   KeyHash<2> hash2;
   KeyHash<1> hash1;
};

template< u32 N, class W > KeyHash<N> &Get( W &w );

template<> KeyHash<1> &Get( Worker &w ) { return w.hash1; }
template<> KeyHash<2> &Get( Worker &w ) { return w.hash2; }
template<> KeyHash<3> &Get( Worker &w ) { return w.hash3; }
template<> KeyHash<4> &Get( Worker &w ) { return w.hash4; }
template<> KeyHash<6> &Get( Worker &w ) { return w.hash6; }
template<> KeyHash<12> &Get( Worker &w ) { return w.hash12; }
template<> KeyHash<18> &Get( Worker &w ) { return w.hash18; }

template< u32 N >
void printcount( Worker *workers, const char *key ) {
   Key<N> find( key );
   u32 count = 0;
   for ( u32 i = 0; i < cpus.count; i++ )
      count += Get<N>( workers[i] )[find];
   cout << count << '\t' << find.tostring() << endl;
}

template<class T>
struct CompareFirst {
   bool operator() ( const T &a, const T &b ) { return a.first < b.first; }
};

template<class T>
struct CompareSecond {
   bool operator() ( const T &a, const T &b ) { return a.second > b.second; }
};


template< u32 N >
void printfreq( Worker *workers ) {
   cout.setf( ios::fixed, ios::floatfield );
   cout.precision( 3 );

   u32 count = 0;
   KeyHash<N> sum;
   for ( u32 i = 0; i < cpus.count; i++ ) {
      KeyHash<N> &hash = Get<N>( workers[i] );
      typename KeyHash<N>::iterator iter = hash.begin(), end = hash.end();
      for ( ; iter != end; ++iter ) {
         count += iter->second;
         sum[iter->first] += iter->second;
      }
   }

   typedef pair< Key<N>, u32 > sequence;
   vector<sequence> seqs( sum.begin(), sum.end() );
   stable_sort( seqs.begin(), seqs.end(), CompareFirst<sequence>() );
   stable_sort( seqs.begin(), seqs.end(), CompareSecond<sequence>() );

   typename vector<sequence>::iterator iter = seqs.begin(), end = seqs.end();
   for ( ; iter != end; ++iter )
      cout <<   iter->first.tostring() << " " << (100.0f * iter->second / count) << endl;
   cout << endl;
}


int main( int argc, const char *argv[] ) {
   Block *block = new Block();
   if ( !block->untilheader( ">THREE" ) )
      return -1;
   block->read();

   Queue workQueue;
   Worker *workers = new Worker[cpus.count];
   for ( u32 i = 0; i < cpus.count; i++ )
      workers[i].start( &workQueue, block );
   for ( u32 i = 0; i < cpus.count; i++ )
      workers[i].join();

   printfreq<1>( workers );
   printfreq<2>( workers );

   printcount<3>( workers, "ggt" );
   printcount<4>( workers, "ggta" );
   printcount<6>( workers, "ggtatt" );
   printcount<12>( workers, "ggtattttaatt" );
   printcount<18>( workers, "ggtattttaatttatagt" );

   delete[] workers;

   return 0;
}
