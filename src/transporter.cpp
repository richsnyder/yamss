#include "yamss/transporter.hpp"

namespace yamss {

transport_error::transport_error(const std::string& a_message)
  : std::runtime_error(a_message)
{
  // empty
}

transporter::transporter()
  : m_curl(NULL)
{
  curl_initialize();
}

transporter::~transporter()
{
  curl_finalize();
}

size_t
transporter::curl_read(void* a_buffer,
                       size_t a_size,
                       size_t a_count,
                       void* a_data)
{
  std::istream* in = (std::istream*) a_data;
  std::streamsize bytes = a_size * a_count;
  in->read((char*) a_buffer, bytes);
  std::streamsize bytes_read = in->gcount();
  return static_cast<size_t>(bytes_read);
}

size_t
transporter::curl_write(void* a_buffer,
                        size_t a_size,
                        size_t a_count,
                        void* a_data)
{
  std::ostream* out = (std::ostream*) a_data;
  std::streamsize bytes = a_size * a_count;
  std::streampos before = out->tellp();
  out->write((char*) a_buffer, bytes);
  std::streamsize bytes_written = out->tellp() - before;
  return static_cast<size_t>(bytes_written);
}

void
transporter::curl_finalize()
{
  if (m_curl)
  {
    curl_easy_cleanup(m_curl);
    m_curl = NULL;
  }
}

void
transporter::curl_initialize()
{
  CURLcode status;

  m_curl = curl_easy_init();
  if (m_curl == NULL)
  {
    std::string msg = "Could not initialize the file transfer system";
    throw transport_error(msg);
  }

  status = curl_easy_setopt(m_curl,
                            CURLOPT_WRITEFUNCTION,
                            transporter::curl_write);
  if (status != CURLE_OK)
  {
    curl_finalize();
    std::string msg = "Could not set the write function to use during downloads";
    throw transport_error(msg);
  }

  status = curl_easy_setopt(m_curl,
                            CURLOPT_READFUNCTION,
                            transporter::curl_read);
  if (status != CURLE_OK)
  {
    curl_finalize();
    std::string msg = "Could not set the read function to use during uploads";
    throw transport_error(msg);
  }
}

void
transporter::get(const path_type& a_local, const url_type& a_remote)
{
  CURLcode status;
  boost::system::error_code boost_status;

  boost::filesystem::path parent = a_local.parent_path();
  if (!parent.empty())
  {
    boost::filesystem::create_directories(a_local.parent_path(), boost_status);
    if (boost_status)
    {
      boost::format fmt("Could not create the parent directory for \"%1%\"");
      throw transport_error(boost::str(fmt % a_local));
    }
  }

  boost::filesystem::ofstream out(a_local);
  if (!out)
  {
    boost::format fmt("Could not open the local file \"%1%\" for writing");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_URL, a_remote.c_str());
  if (status != CURLE_OK)
  {
    out.close();
    boost::filesystem::remove(a_local, boost_status);
    boost::format fmt("Could not set the URL to use to download \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &out);
  if (status != CURLE_OK)
  {
    out.close();
    boost::filesystem::remove(a_local, boost_status);
    boost::format fmt("Could not set the stream to use to download \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 0L);
  if (status != CURLE_OK)
  {
    out.close();
    boost::filesystem::remove(a_local, boost_status);
    std::string msg("Could not prepare the file transfer system for a download");
    throw transport_error(msg);
  }

  status = curl_easy_perform(m_curl);
  if (status != CURLE_OK)
  {
    out.close();
    boost::filesystem::remove(a_local, boost_status);
    boost::format fmt("Failed to download \"%1%\" from \"%2%\"");
    throw transport_error(boost::str(fmt % a_local % a_remote));
  }

  out.close();
}

void
transporter::put(const path_type& a_local, const url_type& a_remote)
{
  CURLcode status;
  boost::system::error_code boost_status;

  bool exists = boost::filesystem::exists(a_local, boost_status);
  if (!exists)
  {
    boost::format fmt("The file \"%1%\" does not exist");
    throw transport_error(boost::str(fmt % a_local));
  }

  curl_off_t file_size = boost::filesystem::file_size(a_local, boost_status);
  if (boost_status)
  {
    boost::format fmt("Could not get the size of the file \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  boost::filesystem::ifstream in(a_local);
  if (!in)
  {
    boost::format fmt("Could not open the local file \"%1%\" for reading");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_URL, a_remote.c_str());
  if (status != CURLE_OK)
  {
    in.close();
    boost::format fmt("Could not set the URL to use to upload \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_READDATA, &in);
  if (status != CURLE_OK)
  {
    in.close();
    boost::format fmt("Could not set the stream to use to upload \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);
  if (status != CURLE_OK)
  {
    in.close();
    std::string msg("Could not prepare the file transfer system for an upload");
    throw transport_error(msg);
  }

  status = curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE, file_size);
  if (status != CURLE_OK)
  {
    in.close();
    boost::format fmt("Could not fix the size of the upload file \"%1%\"");
    throw transport_error(boost::str(fmt % a_local));
  }

  status = curl_easy_perform(m_curl);
  if (status != CURLE_OK)
  {
    in.close();
    boost::format fmt("Failed to upload \"%1%\" to \"%2%\"");
    throw transport_error(boost::str(fmt % a_local % a_remote));
  }

  in.close();
}

} // yamss namespace
