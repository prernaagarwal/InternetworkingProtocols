// UNRELIABLE SENDTO() WRAPPER
// Drops packets using a Bernoulli distribution
//
// Parameters:
// SENDTO_DROP_SIZE - Minimum packet size in bytes to be considered for dropping/duplication
// SENDTO_INITIAL_COUNT - Allow first INITIAL_COUNT packets before consider dropping
// SENDTO_DROP_SIZE - Probability of a packet being dropped (float between 0 and 1)
// SENDTO_DUP_P - Probability of sending a duplicate packet (float between 0 and 1)
// Usage: To enable include -Wl,-wrap=sendto -Wl,-wrap=recvfrom as g++ flags

#ifndef SENDTO_DROP_SIZE
#define SENDTO_DROP_SIZE 1
#endif

#ifndef SENDTO_DROP_P
#define SENDTO_DROP_P 0.1
#endif

#ifndef SENDTO_INTIAL_COUNT
#define SENDTO_INTIAL_COUNT 5
#endif

#ifndef SENDTO_DUP_P
#define SENDTO_DUP_P 0.01
#endif

#ifndef RECVFROM_AVGDELAY_MS
#define RECVFROM_AVGDELAY_MS 75
#endif

//#define _DEBUG_SENDTO_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <random>
#include <chrono>
#include <thread>

#ifdef _DEBUG_SENDTO_
#define _SENDTO_DEBUG_OUT_(...) \
  printf(__VA_ARGS__);		      \
  fflush(stderr);	      
#else
#define _SENDTO_DEBUG_OUT_(...)
#endif


extern "C" ssize_t __real_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

extern "C" ssize_t __real_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

extern "C" ssize_t __wrap_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
  static const double _sendto_drop_p = SENDTO_DROP_P;
  static const double _sendto_dup_p = SENDTO_DUP_P;
  static const size_t _sendto_drop_size = SENDTO_DROP_SIZE;

  static std::random_device _sendto_rd;
  static std::mt19937_64 _sendto_gen(_sendto_rd());
  static std::bernoulli_distribution _sendto_dist(_sendto_drop_p);
  static std::bernoulli_distribution _sendto_dup_dist(_sendto_dup_p);

  static long _sendto_count = SENDTO_INTIAL_COUNT;
  static bool _sendto_first = true;
  
  struct stat statbuf;

  if (_sendto_first == true)
    {
      _sendto_first=false;
      printf("Using __wrap_sendto(): P=%f, Size=%lu bytes, Count=%lu\n",  _sendto_drop_p, _sendto_drop_size, _sendto_count);
    }

  fstat(sockfd, &statbuf);
  
  if (len < _sendto_drop_size || !S_ISSOCK(statbuf.st_mode))
  {
    _SENDTO_DEBUG_OUT_("N");
    return __real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
  }
  else
    {
      if ( _sendto_count > 0)
	{
	  _SENDTO_DEBUG_OUT_("C");
	  _sendto_count --;
	  return __real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
	}
      else
	{
	  if (_sendto_dist(_sendto_gen) == false)
	    {
	      if (_sendto_dup_dist(_sendto_gen) == false)
		{
		  _SENDTO_DEBUG_OUT_("S");
		  return __real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
		}
	      else
		{
		  _SENDTO_DEBUG_OUT_("X");
		  __real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
		  return __real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
		}
	    }
	  else
	    {
	      _SENDTO_DEBUG_OUT_("D");
	      return len;
	    }
	}
    }
}

extern "C" ssize_t __wrap_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
  static const unsigned int _recvfrom_delay_ms = RECVFROM_AVGDELAY_MS;

  static std::random_device _recvfrom_rd;
  static std::mt19937_64 _recvfrom_gen(_recvfrom_rd());
  static std::exponential_distribution<double> _recvfrom_dist( 1/static_cast<double>(_recvfrom_delay_ms));
  static bool _recvfrom_first = true;

  if (_recvfrom_first == true)
    {
      _recvfrom_first=false;
      printf("Using __wrap_recvfrom(): L=%u ms\n", _recvfrom_delay_ms);
    }
  
  if (_recvfrom_delay_ms!=0)
    {
      unsigned int wait=_recvfrom_dist( _recvfrom_gen);
#ifdef _DEBUG_SENDTO_
      std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
#endif

      std::this_thread::sleep_for(std::chrono::milliseconds(wait));

#ifdef _DEBUG_SENDTO_
      std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end-start;
      _SENDTO_DEBUG_OUT_("_W[%f/%u]\n", elapsed.count(), wait);
#endif

      return __real_recvfrom(sockfd,buf,len,flags,src_addr,addrlen);
    }
  else return __real_recvfrom(sockfd,buf,len,flags,src_addr,addrlen);
}
