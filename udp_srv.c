#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "forza.h"

#define USE_NCURSES
#define BUF_SIZE 500

/************************************************************/
struct timeval prevTime, currTime;
int last_raceon = 0;

/************************************************************/
void parse_forza_dbg(unsigned char *message, int len)
{
	int row = 0;
	FORZA_DASH forza;
	memcpy(&forza, message, sizeof(FORZA_DASH));

	for(int i = 0; i < len; i++)
	{
		if(i % 16 == 0)
			printf("\n");
		printf("%02X ",message[i]);
	}
	printf("\n");

	printf("NumCylinders %d\n", forza.NumCylinders);
	
	printf("PositionX %f\n", forza.PositionX);
	printf("PositionY %f\n", forza.PositionY);
	printf("PositionZ %f\n", forza.PositionZ);

	printf("speed %f\n", forza.Speed);

	printf("clutch %d\n", forza.Clutch);
	printf("brake %d\n", forza.Brake);
	printf("accel %d\n", forza.Accel);
	printf("HandBrake %d\n", forza.HandBrake);
}

void parse_dr2_dbg(unsigned char *message, int len)
{
	int row = 0;
	float data_f[17];
	unsigned int data_i[17];
	memcpy(data_f, message, sizeof(float) * 17);
	memcpy(data_i, message, sizeof(int) * 17);

	for(int i = 0; i < len; i++)
	{
		if(i % 16 == 0)
			printf("\n");
		printf("%02X ",message[i]);
	}
	printf("\n--------------\n");

	for(int i = 0; i < len / 4; i++)
		printf("%d %f\n",data_i[i], data_f[i]);
}

void parse_forza(unsigned char *message, int len)
{
	int row = 0;
	FORZA_DASH forza;
	memcpy(&forza, message, sizeof(FORZA_DASH));

	static unsigned int pack_cnt = 0;

	if(last_raceon != forza.IsRaceOn)
	{
#ifdef USE_NCURSES
		clear();
#endif
		last_raceon = forza.IsRaceOn;
	}

	if(forza.IsRaceOn)
	{
		gettimeofday(&currTime, NULL);
		long long int us = currTime.tv_usec - prevTime.tv_usec;
		float ms = us / 1000.0f;
		memcpy(&prevTime, &currTime, sizeof(struct timeval));

#ifdef USE_NCURSES
		mvprintw(row++, 0, "%3.3f ms : let's fly %d\n",ms ,pack_cnt++);

		mvprintw(row++, 0, "RPM %.0f ~ %.0f ~ %.0f", forza.EngineIdleRpm, forza.CurrentEngineRpm, forza.EngineMaxRpm);

		mvprintw(row++, 0, "acc  X %f Y %f Z %f", forza.AccelerationX, forza.AccelerationY, forza.AccelerationZ);
		mvprintw(row++, 0, "vel  X %f Y %f Z %f", forza.VelocityX, forza.VelocityY, forza.VelocityZ);
		mvprintw(row++, 0, "avel X %f Y %f Z %f", forza.AngularVelocityX, forza.AngularVelocityY, forza.AngularVelocityZ);

		mvprintw(row++, 0, "yaw   %f -> %f", forza.Yaw  , forza.Yaw   * 180.0 / M_PI);
		mvprintw(row++, 0, "roll  %f -> %f", forza.Pitch, forza.Pitch * 180.0 / M_PI);
		mvprintw(row++, 0, "pitch %f -> %f", forza.Roll , forza.Roll  * 180.0 / M_PI);

		mvprintw(row++, 0, "NormalizedSuspensionTravelFrontLeft %f FrontRight %f",
			forza.NormalizedSuspensionTravelFrontLeft * 100.0f,
			forza.NormalizedSuspensionTravelFrontRight * 100.0f);
		mvprintw(row++, 0, "NormalizedSuspensionTravelRearLeft %f RearRight %f",
			forza.NormalizedSuspensionTravelRearLeft * 100.0f,
			forza.NormalizedSuspensionTravelRearRight * 100.0f);

		mvprintw(row++, 0, "TireSlipRatioFrontLeft %f FrontRight %f",
			forza.TireSlipRatioFrontLeft * 100.0f,
			forza.TireSlipRatioFrontRight * 100.0f);
		mvprintw(row++, 0, "TireSlipRatioRearLeft %f RearRight %f",
			forza.TireSlipRatioRearLeft * 100.0f,
			forza.TireSlipRatioRearRight * 100.0f);

		mvprintw(row++, 0, "WheelRotationSpeedFrontLeft %f FrontRight %f",
			forza.WheelRotationSpeedFrontLeft * 100.0f,
			forza.WheelRotationSpeedFrontRight * 100.0f);
		mvprintw(row++, 0, "WheelRotationSpeedRearLeft %f RearRight %f",
			forza.WheelRotationSpeedRearLeft * 100.0f,
			forza.WheelRotationSpeedRearRight * 100.0f);

		mvprintw(row++, 0, "WheelOnRumbleStripFrontLeft %f FrontRight %f",
			forza.WheelOnRumbleStripFrontLeft * 100.0f,
			forza.WheelOnRumbleStripFrontRight * 100.0f);
		mvprintw(row++, 0, "WheelOnRumbleStripRearLeft %f RearRight %f",
			forza.WheelOnRumbleStripRearLeft * 100.0f,
			forza.heelOnRumbleStripRearRight * 100.0f);

		mvprintw(row++, 0, "WheelInPuddleDepthFrontLeft %f FrontRight %f",
			forza.WheelInPuddleDepthFrontLeft * 100.0f,
			forza.WheelInPuddleDepthFrontRight * 100.0f);
		mvprintw(row++, 0, "WheelInPuddleDepthRearLeft %f RearRight %f",
			forza.WheelInPuddleDepthRearLeft * 100.0f,
			forza.WheelInPuddleDepthRearRight * 100.0f);

		mvprintw(row++, 0, "SurfaceRumbleFrontLeft %f FrontRight %f",
			forza.SurfaceRumbleFrontLeft * 100.0f,
			forza.SurfaceRumbleFrontRight * 100.0f);
		mvprintw(row++, 0, "SurfaceRumbleRearLeft %f RearRight %f",
			forza.SurfaceRumbleRearLeft * 100.0f,
			forza.SurfaceRumbleRearRight * 100.0f);

		mvprintw(row++, 0, "TireSlipAngleFrontLeft %f FrontRight %f",
			forza.TireSlipAngleFrontLeft * 100.0f,
			forza.TireSlipAngleFrontRight * 100.0f);
		mvprintw(row++, 0, "TireSlipAngleRearLeft %f RearRight %f",
			forza.TireSlipAngleRearLeft * 100.0f,
			forza.TireSlipAngleRearRight * 100.0f);

		mvprintw(row++, 0, "TireCombinedSlipFrontLeft %f FrontRight %f",
			forza.TireCombinedSlipFrontLeft * 100.0f,
			forza.TireCombinedSlipFrontRight * 100.0f);
		mvprintw(row++, 0, "TireCombinedSlipRearLeft %f RearRight %f",
			forza.TireCombinedSlipRearLeft * 100.0f,
			forza.TireCombinedSlipRearRight * 100.0f);

		mvprintw(row++, 0, "SuspensionTravelMetersFrontLeft %f FrontRight %f",
			forza.SuspensionTravelMetersFrontLeft * 100.0f,
			forza.SuspensionTravelMetersFrontRight * 100.0f);
		mvprintw(row++, 0, "SuspensionTravelMetersRearLeft %f RearRight %f",
			forza.SuspensionTravelMetersRearLeft * 100.0f,
			forza.SuspensionTravelMetersRearRight * 100.0f);

		mvprintw(row++, 0, "clutch[%3d] brake[%3d] accel[%3d] handbrake[%3d] gear[%2d] Steer[%4d]"
		, forza.Clutch
		, forza.Brake
		, forza.Accel
		, forza.HandBrake
		, forza.Gear
		, forza.Steer
		);

		mvprintw(row++, 0, "CarOrdinal %5d CarClass %5d CarPerformanceIndex %5d DrivetrainType %5d NumCylinders %5d",
			forza.CarOrdinal, forza.CarClass, forza.CarPerformanceIndex, forza.DrivetrainType, forza.NumCylinders);

		mvprintw(row++, 0, "Position %f , %f , %f"
		, forza.PositionX
		, forza.PositionY
		, forza.PositionZ
		);

		mvprintw(row++, 0, "Speed[%5.0f] Power[%7.0f] Torque[%5.1f] Boost[%5.1f] Fuel[%5.0f]"
		, forza.Speed
		, forza.Power / 1000.0f
		, forza.Torque / 10.0f
		, forza.Boost
		, forza.Fuel
		);

		mvprintw(row++, 0, "TireTempFrontLeft %f FrontRight %f",
			forza.TireTempFrontLeft,
			forza.TireTempFrontRight);
		mvprintw(row++, 0, "TireTempRearLeft %f RearRight %f",
			forza.TireTempRearLeft,
			forza.TireTempRearRight);

		mvprintw(row++, 0, "TireWearFrontLeft %f FrontRight %f",
			forza.TireWearFrontLeft,
			forza.TireWearFrontRight);
		mvprintw(row++, 0, "TireWearRearLeft %f RearRight %f",
			forza.TireWearRearLeft,
			forza.TireWearRearRight);
#endif
	}
	else
	{
#ifdef USE_NCURSES
		mvprintw(row++, 0, "pause");
#endif
	}

	refresh();
}

void error_handling(char *message);

int main(int argc, char *argv[]){
	int serv_sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;

	// printf("%d sizeof(FORZA_SLED)\n", sizeof(FORZA_SLED));
	// printf("%d sizeof(FORZA_DASH)\n", sizeof(FORZA_DASH));

	if(argc != 2)
	{
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket creation error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

#ifdef USE_NCURSES
	initscr();
#endif

	gettimeofday(&currTime, NULL);
	memcpy(&prevTime, &currTime, sizeof(struct timeval));

	while(1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		message[str_len] = 0x00;
		// printf("%d received [%s]\n", str_len, message);

#ifdef USE_NCURSES
		parse_forza(message, str_len);
#else
		// parse_dr2_dbg(message, str_len);
		parse_forza_dbg(message, str_len);
#endif
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);

#ifdef USE_NCURSES
	endwin();
#endif
	exit(1);
}


