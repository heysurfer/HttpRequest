#include "HttpRequest.hpp"

int main()
{
  if (DownloadFile("URL", "LOCATION"))
    printf("Success Download\n");
  else
    printf("Failed To Download\n");
}
