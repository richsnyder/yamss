/** @file
 *
 *  This file defines a class that can be used to transport a file between two
 *  network locations.
 *
 *  @brief Transport files over the network.
 */
#ifndef YAMSS_TRANSPORTER_HPP
#define YAMSS_TRANSPORTER_HPP

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <curl/curl.h>

namespace yamss {

/** The `transporter` class uses this exception class to report errors that may
 *  occur while transferring a file over the network.
 *
 *  @brief Exception class that indicates a file transport error.
 *  @see transporter
 */
struct transport_error : public std::runtime_error
{
  transport_error(const std::string& a_message);
}; // transport_error struct

/** A mechanism for transporting files over the network.
 *
 *  This class uses the cURL multiprotocol file transfer library under the
 *  hood.  Please refer to its documentation at <http://curl.haxx.se/libcurl>
 *  for more information.
 *
 *  @brief Transport files over the network.
 */
class transporter
{
public:
  /** The cURL library function `curl_easy_setopt` must be able to access the
   *  `curl_read` and `curl_write` methods that are defined by this class.
   *
   *  @brief Declare cURL's easy option setting method to be a friend.
   */
  friend CURLcode curl_easy_setopt(CURL*, CURLoption, ...);

  /** Type used to represent a path on the local filesystem.
   *
   *  @brief Type used to represent a file path.
   */
  typedef boost::filesystem::path path_type;

  /** Type used to represent a Uniform Resource Locator (URL).
   *
   *  @brief Type used to represent a URL.
   */
  typedef std::string url_type;

  /** Create a new file transporter.
   *
   *  @brief Default constructor.
   */
  transporter();

  /** Destroy a file transporter.
   *
   *  @brief Destructor.
   */
  ~transporter();

  /** Copy a file from a remote location and place it on the local filesystem.
   *  The remote file is identified by a URL, as described by RFC 3986.
   *  Supported schemes include HTTP, FTP, SMTP, POP3, IMAP, SCP, SFTP, LDAP,
   *  and RTMP.
   *
   *  @brief Transfer a remote file to the local filesystem.
   *
   *  @param[in] a_local
   *      The file that is retrieved from the remote location will be saved to
   *      the local filesystem at this path, which may be either absolute or
   *      relative.
   *  @param[in] a_remote
   *      A URL to a remote file.
   *  @exception transport_error
   *      Thrown if an error occurs while transporting the file.
   */
  void
  get(const path_type& a_local, const url_type& a_remote);

  /** Copy a file on the local filesystem to a remote location.  The remote
   *  location is identified by a URL, as described by RFC 3986.  Supported
   *  schemes include HTTP, FTP, SMTP, POP3, IMAP, SCP, SFTP, LDAP, and RTMP.
   *
   *  @brief Transfer a local file to a remote filesystem.
   *
   *  @param[in] a_local
   *      A path to a local file.  This may be either absolute or relative.
   *  @param[in] a_remote
   *      The file that is retrieved from the local filesystem will be saved to
   *      the remote filesystem according to this URL.
   *  @exception transport_error
   *      Thrown if an error occurs while transporting the file.
   */
  void
  put(const path_type& a_local, const url_type& a_remote);
protected:
  /** Initialize the cURL system.
   *
   *  @brief Initialize cURL.
   *
   *  @exception transport_error
   *      Thrown if an error occurs while initializing the cURL system.
   */
  void
  curl_initialize();

  /** Finalize the cURL system.
   *
   *  @brief Finalize cURL.
   */
  void
  curl_finalize();

  /** This method is used by cURL to read at most `a_count` elements of size
   *  `a_size` bytes from an input data stream, `a_data`.  The elements are
   *  stored in `a_buffer` and the function returns the number of bytes that
   *  were actually read off of the stream.
   *
   *  @brief The cURL read function.
   *
   *  @param[in] a_buffer
   *      A buffer used to store elements read from the input data stream.
   *  @param[in] a_size
   *      The number of bytes used for each element that is to be read.
   *  @param[in] a_count
   *      The number of elements to read from the input data stream.
   *  @param[in] a_data
   *      A pointer to an input data stream.
   *  @return
   *      The number of bytes read from the data stream.
   *  @exception transport_error
   *      Thrown if an error occurs while reading from the stream.
   */
  static
  size_t
  curl_read(void* a_buffer, size_t a_size, size_t a_count, void* a_data);

  /** This method is used by cURL to write at most `a_count` elements of size
   *  `a_size` bytes to an output data stream, `a_data`.  The elements are
   *  retrieved from `a_buffer` and the function returns the number of bytes
   *  that were actually written to the stream.
   *
   *  @brief The cURL write function.
   *
   *  @param[in] a_buffer
   *      A buffer that contains elements to be written to the output data
   *      stream.
   *  @param[in] a_size
   *      The number of bytes used for each element that is to be written.
   *  @param[in] a_count
   *      The number of elements to write to the output data stream.
   *  @param[in] a_data
   *      A pointer to an output data stream.
   *  @return
   *      The number of bytes written to the data stream.
   *  @exception transport_error
   *      Thrown if an error occurs while writing to the stream.
   */
  static
  size_t
  curl_write(void* a_buffer, size_t a_size, size_t a_count, void* a_data);
private:
  CURL* m_curl;
}; // transporter class

} // yamss namespace

#endif // YAMSS_TRANSPORTER_HPP
