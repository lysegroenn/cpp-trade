#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <memory>
#include "json.hpp"
#include <unistd.h>
#include <vector>

using json = nlohmann::json;





class LyseClient
{
    public:
        LyseClient()
        {


            // Get a handle
            curl = curl_easy_init();
            std::cout << "Initialized class" << std::endl;
        }

        std::vector<std::string> getPosts()
        {
            std::vector<std::string> postsPtr;

            if(curl)
            {
                    // Set target URL
                    curl_easy_setopt(curl, CURLOPT_URL, "https://www.lysegroenn.com/api/test/posts");

                    // Specify GET method
                    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

                    // Response information.
                    long httpCode(0);
                    std::unique_ptr<std::string> httpData(new std::string());


                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);


                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

                    // Make request
                    res = curl_easy_perform(curl);
                    // Get http response
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

                    std::cout << httpCode << std::endl;

                if(httpCode == 200)
                {

                    json j;
                    try
                    {
                        j = json::parse(*httpData.get());

                        for(auto it : j["posts"])
                        {
                            //std::cout << it["title"] << std::endl;
                            postsPtr.push_back(it["title"]);
                        }
                        //std::cout << postsPtr[0] << std::endl;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                } else
                {
                    std::cout << httpCode << std::endl;
                }
                return postsPtr;

            }
            

            
            // Check for errors
            if(res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed %s\n", curl_easy_strerror(res));
            }
            
            // Cleanup
            //curl_easy_cleanup(curl);
        }

        int makePost(std::string title)
        {
            if(curl)
            {
                    // Set target URL
                    curl_easy_setopt(curl, CURLOPT_URL, "https://www.lysegroenn.com/api/test/userPosts");

                    // Specify GET method
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "title=fromCPP");

                    // Response information.
                    long httpCode(0);

                    // Make request
                    res = curl_easy_perform(curl);
                    // Get http response
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

                    std::cout << httpCode << std::endl;
   
                    //curl_easy_cleanup(curl);

                    if(httpCode == 201)
                    {
                        std::cout << "Post created" << std::endl;
                        return 0;
                    } else 
                    {
                        std::cout << httpCode << std::endl;
                        return 1;
                    }
            }
        }

        void cleanup()
        {
            curl_easy_cleanup(curl);
        } 

    private:
        CURL *curl;
        CURLcode res;
        static std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out)
            {
                const std::size_t totalBytes(size * num);
                out->append(in, totalBytes);
                return totalBytes;
            }      
};


int main(int argc, char** argv)
{
     

    curl_global_init(CURL_GLOBAL_ALL);

    LyseClient Adam;
    Adam.makePost("hej");
    std::vector<std::string> Posts = Adam.getPosts();

    for(int i = 0; i < Posts.size(); i++)
    {
        std::cout << Posts[i] << std::endl;
    }
    Adam.cleanup();

    curl_global_cleanup();
    return 0;
}
