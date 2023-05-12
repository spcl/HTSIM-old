/** @file cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.23
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "goalsim"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "goalsim"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "0.1"
#endif

/** @brief Where the command line options are stored */
struct gengetopt_args_info {
    const char *help_help;     /**< @brief Print help and exit help description.  */
    const char *version_help;  /**< @brief Print version and exit help description.  */
    char *filename_arg;        /**< @brief Prefix for the filenames which contain the schedules.  */
    char *filename_orig;       /**< @brief Prefix for the filenames which contain the schedules original value given at
                                  command line.  */
    const char *filename_help; /**< @brief Prefix for the filenames which contain the schedules help description.  */
    int save_mem_flag; /**< @brief Map the schedules as MAP_SHARED to enable processing of large schedule (larger than
                          main memory). This will invalidate the schedules during simulation. (default=off).  */
    const char *save_mem_help; /**< @brief Map the schedules as MAP_SHARED to enable processing of large schedule
                                  (larger than main memory). This will invalidate the schedules during simulation. help
                                  description.  */
    int LogGOPS_L_arg;         /**< @brief The latency parameter L in the LogGP model (default='2500').  */
    char *LogGOPS_L_orig; /**< @brief The latency parameter L in the LogGP model original value given at command line.
                           */
    const char *LogGOPS_L_help; /**< @brief The latency parameter L in the LogGP model help description.  */
    int LogGOPS_o_arg;          /**< @brief The overhead parameter o in the LogGP model (default='1500').  */
    char *LogGOPS_o_orig; /**< @brief The overhead parameter o in the LogGP model original value given at command line.
                           */
    const char *LogGOPS_o_help; /**< @brief The overhead parameter o in the LogGP model help description.  */
    int LogGOPS_g_arg;          /**< @brief The gap per message parameter g in the LogGP model (default='1000').  */
    char *LogGOPS_g_orig; /**< @brief The gap per message parameter g in the LogGP model original value given at command
                             line.  */
    const char *LogGOPS_g_help; /**< @brief The gap per message parameter g in the LogGP model help description.  */
    int LogGOPS_G_arg;          /**< @brief The gap per byte parameter G in the LogGP model (default='6').  */
    char *LogGOPS_G_orig; /**< @brief The gap per byte parameter G in the LogGP model original value given at command
                             line.  */
    const char *LogGOPS_G_help; /**< @brief The gap per byte parameter G in the LogGP model help description.  */
    int LogGOPS_S_arg; /**< @brief Datasize at which we change from eager to rendezvous protocol (default='65535').  */
    char *LogGOPS_S_orig; /**< @brief Datasize at which we change from eager to rendezvous protocol original value given
                             at command line.  */
    const char *LogGOPS_S_help; /**< @brief Datasize at which we change from eager to rendezvous protocol help
                                   description.  */
    int LogGOPS_O_arg;          /**< @brief The overhead per byte in LogGOP (default='0').  */
    char *LogGOPS_O_orig;       /**< @brief The overhead per byte in LogGOP original value given at command line.  */
    const char *LogGOPS_O_help; /**< @brief The overhead per byte in LogGOP help description.  */
    char *vizfile_arg;          /**< @brief Name of the output file for visualization data.  */
    char *vizfile_orig; /**< @brief Name of the output file for visualization data original value given at command line.
                         */
    const char *vizfile_help;     /**< @brief Name of the output file for visualization data help description.  */
    const char *verbose_help;     /**< @brief Enable more verbose output help description.  */
    const char *progress_help;    /**< @brief print progress help description.  */
    const char *batchmode_help;   /**< @brief enable batchmode (never print detailed host info) help description.  */
    char *noise_trace_arg;        /**< @brief Read Noise from trace <file>.  */
    char *noise_trace_orig;       /**< @brief Read Noise from trace <file> original value given at command line.  */
    const char *noise_trace_help; /**< @brief Read Noise from trace <file> help description.  */
    int noise_cosched_flag;       /**< @brief Co-schedule noise (use same starttime on all processes) (default=off).  */
    const char *
            noise_cosched_help; /**< @brief Co-schedule noise (use same starttime on all processes) help description. */
    char *network_type_arg;     /**< @brief Network type (LogGP=no network congestion; simple=simple linear model)
                                   (default='LogGP').  */
    char *network_type_orig; /**< @brief Network type (LogGP=no network congestion; simple=simple linear model) original
                                value given at command line.  */
    const char *network_type_help; /**< @brief Network type (LogGP=no network congestion; simple=simple linear model)
                                      help description.  */
    char *network_file_arg;        /**< @brief Input file for network (annotated dot format).  */
    char *network_file_orig; /**< @brief Input file for network (annotated dot format) original value given at command
                                line.  */
    const char *network_file_help; /**< @brief Input file for network (annotated dot format) help description.  */
    char *qstat_arg;  /**< @brief Enable PQ and UQ statistics.  Argument is output filename prefix (default='Unknown').
                       */
    char *qstat_orig; /**< @brief Enable PQ and UQ statistics.  Argument is output filename prefix original value given
                         at command line.  */
    const char *qstat_help; /**< @brief Enable PQ and UQ statistics.  Argument is output filename prefix help
                               description.  */

    unsigned int help_given;          /**< @brief Whether help was given.  */
    unsigned int version_given;       /**< @brief Whether version was given.  */
    unsigned int filename_given;      /**< @brief Whether filename was given.  */
    unsigned int save_mem_given;      /**< @brief Whether save-mem was given.  */
    unsigned int LogGOPS_L_given;     /**< @brief Whether LogGOPS_L was given.  */
    unsigned int LogGOPS_o_given;     /**< @brief Whether LogGOPS_o was given.  */
    unsigned int LogGOPS_g_given;     /**< @brief Whether LogGOPS_g was given.  */
    unsigned int LogGOPS_G_given;     /**< @brief Whether LogGOPS_G was given.  */
    unsigned int LogGOPS_S_given;     /**< @brief Whether LogGOPS_S was given.  */
    unsigned int LogGOPS_O_given;     /**< @brief Whether LogGOPS_O was given.  */
    unsigned int vizfile_given;       /**< @brief Whether vizfile was given.  */
    unsigned int verbose_given;       /**< @brief Whether verbose was given.  */
    unsigned int progress_given;      /**< @brief Whether progress was given.  */
    unsigned int batchmode_given;     /**< @brief Whether batchmode was given.  */
    unsigned int noise_trace_given;   /**< @brief Whether noise-trace was given.  */
    unsigned int noise_cosched_given; /**< @brief Whether noise-cosched was given.  */
    unsigned int network_type_given;  /**< @brief Whether network-type was given.  */
    unsigned int network_file_given;  /**< @brief Whether network-file was given.  */
    unsigned int qstat_given;         /**< @brief Whether qstat was given.  */
};

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params {
    int override;        /**< @brief whether to override possibly already present options (default 0) */
    int initialize;      /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
    int check_required;  /**< @brief whether to check that all required options were provided (default 1) */
    int check_ambiguity; /**< @brief whether to check for options already specified in the option structure
                            gengetopt_args_info (default 0) */
    int print_errors;    /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
};

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief the description string of the program */
extern const char *gengetopt_args_info_description;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser(int argc, char **argv, struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2(int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize,
                    int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext(int argc, char **argv, struct gengetopt_args_info *args_info,
                       struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init(struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free(struct gengetopt_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required(struct gengetopt_args_info *args_info, const char *prog_name);

extern const char *cmdline_parser_network_type_values[]; /**< @brief Possible values for network-type. */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
