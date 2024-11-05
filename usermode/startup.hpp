#include "render.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "json.hpp"
#include "cheat-func.h"
#include "httplib.h"

//scroll down 

using json = nlohmann::json;


std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static auto cr3loop() -> void
{
    for (;;)
    {
        mem::GetDir();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}



class startup1
{
public:
    void Startup()
    {
        httplib::Server svr;

        svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
            try {

                std::string html = read_file("ui/index.html");
                res.set_content(html, "text/html");
            }
            catch (const std::exception& e) {
                res.status = 500;
                res.set_content("Internal Server Error: Could not load HTML file.", "text/plain");
            }
            });

        svr.Post("/enable_aimbot", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "Aimbot enabled" << std::endl;
            enable_aimbot();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/distance", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "distance enabled" << std::endl;
            distance();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/snapline", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "snapline enabled" << std::endl;
            snapline();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/username", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "username enabled" << std::endl;
            username();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/ranked", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "ranked enabled" << std::endl;
            ranked();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_head", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "Head enabled" << std::endl;
            enable_head();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/outline", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "outline enabled" << std::endl;
            outline();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_prediction", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "Aimbot enabled" << std::endl;
            enable_prediction();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_fov", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "fovv enabled" << std::endl;
            enable_fov();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_vsync", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "vsync enabled" << std::endl;
            enable_vsync();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_crosshair", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "crosshair enabled" << std::endl;
            enable_crosshair();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_watermark", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "watermark enabled" << std::endl;
            enable_watermark();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/enable_skeleton", [](const httplib::Request& req, httplib::Response& res) {
            std::cout << "Skeleton enabled" << std::endl;
            enable_skeleton();
            res.set_content(R"({"status": "success"})", "application/json");
            });

        svr.Post("/set_box_esp_flags", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                globals::g_box_esp = json["box_esp"].get<bool>(); 
                globals::g_enable_esp = json["enable_esp"].get<bool>();

                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            });

        svr.Post("/set_fov", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                int fov = json["fov"].get<int>();
                std::cout << "fov: " << fov << std::endl;
                set_fov(fov);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            });

        svr.Post("/renderdistance", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                if (!json.contains("distance") || json["distance"].is_null()) {
                    throw std::invalid_argument("Missing or null 'distance' value");
                }
                int distance = json["distance"].get<int>();
                std::cout << "distance: " << distance << std::endl;
                renderdistance(distance);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            catch (const std::invalid_argument& e) {
                std::cout << "Invalid input: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})", "application/json");
            }
            });

        svr.Post("/boxthickness", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                if (!json.contains("boxthic") || json["boxthic"].is_null()) {
                    throw std::invalid_argument("Missing or null 'boxthic' value");
                }
                int boxthic = json["boxthic"].get<int>();
                std::cout << "boxthic: " << boxthic << std::endl;
                boxthickness(boxthic);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            catch (const std::invalid_argument& e) {
                std::cout << "Invalid input: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})", "application/json");
            }
            });

        svr.Post("/skelthickness", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                if (!json.contains("skelthick") || json["skelthick"].is_null()) {
                    throw std::invalid_argument("Missing or null 'skelthick' value");
                }
                int skelthick = json["skelthick"].get<int>();
                std::cout << "skelthick: " << skelthick << std::endl;
                skelthickness(skelthick);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            catch (const std::invalid_argument& e) {
                std::cout << "Invalid input: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})", "application/json");
            }
            });

        svr.Post("/snapthickness", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                if (!json.contains("snapthick") || json["snapthick"].is_null()) {
                    throw std::invalid_argument("Missing or null 'snapthick' value");
                }
                int snapthick = json["snapthick"].get<int>();
                std::cout << "snapthick: " << snapthick << std::endl;
                snapthickness(snapthick);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            catch (const std::invalid_argument& e) {
                std::cout << "Invalid input: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})", "application/json");
            }
            });




        svr.Post("/set_smooth", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto json = json::parse(req.body);
                if (!json.contains("smooth") || json["smooth"].is_null()) {
                    throw std::invalid_argument("Missing or null 'smooth' value");
                }
                int smooth = json["smooth"].get<int>();
                std::cout << "smooth: " << smooth << std::endl;
                set_smooth(smooth);
                res.set_content(R"({"status": "success"})", "application/json");
            }
            catch (const json::exception& e) {
                std::cout << "Error parsing JSON: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Invalid JSON"})", "application/json");
            }
            catch (const std::invalid_argument& e) {
                std::cout << "Invalid input: " << e.what() << std::endl;
                res.status = 400;
                res.set_content(R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})", "application/json");
            }
            });


        //main shit


        std::cout << "Server started on http://localhost:8080" << std::endl;
        std::thread server_thread([&svr]() {
            svr.listen("localhost", 8080);
            });
        server_thread.detach();

        //Input.init(); no mouse no aimbob for you

        auto result = mem::GetDrv();

        screen_width = GetSystemMetrics(SM_CXSCREEN);
        screen_height = GetSystemMetrics(SM_CYSCREEN);

        while (windows == NULL)
        {
            XorS(wind, "Fortnite  ");
            windows = FindWindowA(0, wind.decrypt());
        }

        auto pid = mem::find_process(L"FortniteClient-Win64-Shipping.exe");

        auto virtualaddy = mem::GetBase();

        baseaddress = virtualaddy;

        mem::GetDir();

        std::printf("base address -> %I64d\n", baseaddress);

        std::thread([&]() { for (;;) { g_cache->actors();  cr3loop(); } }).detach();


        g_render->Overlay();
    }
};

static startup1* startup = new startup1;
