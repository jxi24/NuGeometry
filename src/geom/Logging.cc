#include "geom/Logging.hh"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void CreateLogger(bool to_file, int level, int flush_time) {
    auto slevel = static_cast<spdlog::level::level_enum>(level);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(slevel);

    std::shared_ptr<spdlog::logger> logger;
    if(to_file) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("geom.log", true);
        file_sink->set_level(slevel);

        spdlog::sinks_init_list sink_list = spdlog::sinks_init_list{file_sink, console_sink};
        logger = std::make_shared<spdlog::logger>("nugeom", sink_list);
    } else {
        spdlog::sinks_init_list sink_list = spdlog::sinks_init_list{console_sink};
        logger = std::make_shared<spdlog::logger>("nugeom", sink_list);
    }
    logger -> set_level(slevel);
    logger -> flush_on(spdlog::level::warn);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(flush_time));
}
