//============================================================================
// Name        : TurtleBotSpeechRecognition.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <pocketsphinx/pocketsphinx.h>
#include <sphinxbase/jsgf.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/err.h>
#include <inttypes.h>

using namespace std;

ps_decoder_t *ps;
cmd_ln_t *config;
FILE *fh;
char const *hyp, *uttid;
int16 buf[512];
int rv;
int32 score;

ad_rec_t *ad;
int16 adbuf[1024];
uint8 utt_started, in_speech;
int32 k;

///* Sleep for specified msec */
//static void
//sleep_msec(int32 ms)
//{
//#if (defined(_WIN32) && !defined(GNUWINCE)) || defined(_WIN32_WCE)
//    Sleep(ms);
//#else
//    /* ------------------- Unix ------------------ */
//    struct timeval tmo;
//
//    tmo.tv_sec = 0;
//    tmo.tv_usec = ms * 1000;
//
//    select(0, NULL, NULL, NULL, &tmo);
//#endif
//}
//
///*
// * Main utterance processing loop:
// *     for (;;) {
// *        start utterance and wait for speech to process
// *        decoding till end-of-utterance silence will be detected
// *        print utterance result;
// *     }
// */
//static void
//recognize_from_microphone()
//{
//    ad_rec_t *ad;
//    int16 adbuf[2048];
//    uint8 utt_started, in_speech;
//    int32 k;
//    char const *hyp;
//
//    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
//                          (int) cmd_ln_float32_r(config,
//                                                 "-samprate"))) == NULL)
//        E_FATAL("Failed to open audio device\n");
//    if (ad_start_rec(ad) < 0)
//        E_FATAL("Failed to start recording\n");
//
//    if (ps_start_utt(ps) < 0)
//        E_FATAL("Failed to start utterance\n");
//    utt_started = FALSE;
//    E_INFO("Ready....\n");
//
//    for (;;) {
//        if ((k = ad_read(ad, adbuf, 2048)) < 0)
//            E_FATAL("Failed to read audio\n");
//        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
//        in_speech = ps_get_in_speech(ps);
//        if (in_speech && !utt_started) {
//            utt_started = TRUE;
//            E_INFO("Listening...\n");
//        }
//        if (!in_speech && utt_started) {
//            /* speech -> silence transition, time to start new utterance  */
//            ps_end_utt(ps);
//            hyp = ps_get_hyp(ps, NULL );
//            if (hyp != NULL) {
//                printf("%s\n", hyp);
//                fflush(stdout);
//            }
//
//            if (ps_start_utt(ps) < 0)
//                E_FATAL("Failed to start utterance\n");
//            utt_started = FALSE;
//            E_INFO("Ready....\n");
//        }
//        sleep_msec(100);
//    }
//    ad_close(ad);
//}

void send_ros_command(char const* hyp) {

}


void extract_command(char const* hyp, int32 score) {
	if(hyp == NULL) {
		return;
	}
//
//public <command> = turtle bot go (<location> | <movement>);
//
//private <location> = to (the lab | the kitchen);
//private <movement> = forward | backwards;

	if(score > -8000) {
//		if((strcmp (hyp,"turtle bot go to the lab") == 0)) {
//			printf("COMMAND LAB\n");
//			send_ros_command(hyp);
//		} else if((strcmp (hyp,"turtle bot go to the kitchen") == 0)) {
//			printf("COMMAND KITCHEN\n");
//			send_ros_command(hyp);
//		} else if((strcmp (hyp,"turtle bot go forward") == 0)) {
//			printf("COMMAND FORWARD\n");
//			send_ros_command(hyp);
//		} else if((strcmp (hyp,"turtle bot go backwards") == 0)) {
//			printf("COMMAND BACKWARDS\n");
//			send_ros_command(hyp);
//		} else {
//			printf("ABSOLUTE MIND FUCK\n");
//		}

		if((strcmp (hyp,"go lab") == 0)) {
				printf("COMMAND LAB\n");
				send_ros_command(hyp);
			} else if((strcmp (hyp,"go kitchen") == 0)) {
				printf("COMMAND KITCHEN\n");
				send_ros_command(hyp);
			} else if((strcmp (hyp,"go forward") == 0)) {
				printf("COMMAND FORWARD\n");
				send_ros_command(hyp);
			} else if((strcmp (hyp,"go backwards") == 0)) {
				printf("COMMAND BACKWARDS\n");
				send_ros_command(hyp);
			} else {
				printf("ABSOLUTE MIND FUCK\n");
			}
	} else {
		printf("COMMAND DENIED\n");
	}
}

string recognize_from_microphone() {
	ad_start_rec(ad);
	ps_start_utt(ps);
	utt_started = FALSE;

	while(1) {

		try {;

			k = ad_read(ad, adbuf, 1024); //blockiert
			int rawCode = ps_process_raw(ps, adbuf, k, FALSE, FALSE);

			in_speech = ps_get_in_speech(ps);

			if(in_speech && !utt_started) {
				utt_started = TRUE;
			}

			if(!in_speech && utt_started) {
				ps_end_utt(ps);
				ad_stop_rec(ad);

				utt_started = FALSE;

				int32 score = 30;


				hyp = ps_get_hyp(ps, &score);

				printf("SCORE %d\n",score);
				fflush(stdout);

				extract_command(hyp, score);

				return hyp;

				break;
			}

		} catch (const std::exception& e) {
			printf("Exception caught.\n\n");
			fflush(stdout);
			return "";
			break;
		}



	}
}

int main(int argc, char *argv[]) {

	config = cmd_ln_init(NULL, ps_args(), TRUE,
			"-hmm", MODELDIR "/en-us/en-us",
			//"-lm", MODELDIR "/en-us/en-us.lm.bin",
			"-dict", MODELDIR "/en-us/cmudict-en-us.dict",
			"-jsgf", "./grammar.gram",
			NULL);

	if (config == NULL) {
		fprintf(stderr,
				"Failed to create config object, see log for details\n");
		return -1;
	}
		fflush(stdout);

	ps = ps_init(config);
	if (ps == NULL) {
		fprintf(stderr, "Failed to create recognizer, see log for details\n");
		return -1;
	}

	ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), (int) cmd_ln_float32_r(config, "-samprate"));

	while(1) {
		string decoded_speech = recognize_from_microphone();
		cout << "Decoded Speech: " << decoded_speech << "\n" << endl;
	}

	ad_close(ad);

//	fh = fopen("Roboter_go_to_the_lab_02_ohne.raw", "rb");
//	if (fh == NULL) {
//		fprintf(stderr, "Unable to open input file goforward.raw\n");
//		return -1;
//	}
//
//	rv = ps_start_utt(ps);
//
//	while (!feof(fh)) {
//		size_t nsamp;
//		nsamp = fread(buf, 2, 512, fh);
//		rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
//	}
//
//	rv = ps_end_utt(ps);
//	hyp = ps_get_hyp(ps, &score);
//	printf("Recognized: %s\n", hyp);
//
//	fclose(fh);
//	ps_free(ps);
//	cmd_ln_free_r(config);

	return 0;
}




