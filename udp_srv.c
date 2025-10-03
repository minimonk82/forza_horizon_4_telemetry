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
#define GRAVITY  9.80665

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
#endif
	}
	else
	{
#ifdef USE_NCURSES
		mvprintw(row++, 0, "pause");
		// refresh();
		return ;
#endif
	}

#ifdef USE_NCURSES
	mvprintw(row++, 0, "RPM %5.0f ~ %5.0f ~ %5.0f", forza.EngineIdleRpm, forza.CurrentEngineRpm, forza.EngineMaxRpm);

	mvprintw(row++, 0, "acc  X %10.5f Y %10.5f Z %10.5f", forza.AccelerationX / GRAVITY, forza.AccelerationY / GRAVITY, forza.AccelerationZ / GRAVITY);
	// mvprintw(row++, 0, "acc  X %10.5f Y %10.5f Z %10.5f", forza.AccelerationX / GRAVITY, forza.AccelerationY / GRAVITY, forza.AccelerationZ / GRAVITY);
	mvprintw(row++, 0, "vel  X %10.5f Y %10.5f Z %10.5f", forza.VelocityX, forza.VelocityY, forza.VelocityZ);
	mvprintw(row++, 0, "avel X %10.5f Y %10.5f Z %10.5f", forza.AngularVelocityX, forza.AngularVelocityY, forza.AngularVelocityZ);

	mvprintw(row++, 0, "yaw   %8.5f -> %8.5f", forza.Yaw  , forza.Yaw   * 180.0 / M_PI);
	mvprintw(row++, 0, "roll  %8.5f -> %8.5f", forza.Pitch, forza.Pitch * 180.0 / M_PI);
	mvprintw(row++, 0, "pitch %8.5f -> %8.5f", forza.Roll , forza.Roll  * 180.0 / M_PI);

	mvprintw(row++, 0, "NormalizedSuspensionTravelFrontLeft %5.1f FrontRight %5.1f",
		forza.NormalizedSuspensionTravelFrontLeft * 100.0f,
		forza.NormalizedSuspensionTravelFrontRight * 100.0f);
	mvprintw(row++, 0, "NormalizedSuspensionTravelRearLeft %5.1f RearRight %5.1f",
		forza.NormalizedSuspensionTravelRearLeft * 100.0f,
		forza.NormalizedSuspensionTravelRearRight * 100.0f);

	mvprintw(row++, 0, "TireSlipRatioFrontLeft %6.1f FrontRight %6.1f",
		forza.TireSlipRatioFrontLeft * 100.0f,
		forza.TireSlipRatioFrontRight * 100.0f);
	mvprintw(row++, 0, "TireSlipRatioRearLeft %6.1f RearRight %6.1f",
		forza.TireSlipRatioRearLeft * 100.0f,
		forza.TireSlipRatioRearRight * 100.0f);

	mvprintw(row++, 0, "WheelRotationSpeedFrontLeft %5.1f FrontRight %5.1f",
		forza.WheelRotationSpeedFrontLeft * 180.0 / M_PI,
		forza.WheelRotationSpeedFrontRight * 180.0 / M_PI);
	mvprintw(row++, 0, "WheelRotationSpeedRearLeft %5.1f RearRight %5.1f",
		forza.WheelRotationSpeedRearLeft * 180.0 / M_PI,
		forza.WheelRotationSpeedRearRight * 180.0 / M_PI);

	// mvprintw(row++, 0, "WheelOnRumbleStripFrontLeft %5.1f FrontRight %5.1f",
	// 	forza.WheelOnRumbleStripFrontLeft * 100.0f,
	// 	forza.WheelOnRumbleStripFrontRight * 100.0f);
	// mvprintw(row++, 0, "WheelOnRumbleStripRearLeft %5.1f RearRight %5.1f",
	// 	forza.WheelOnRumbleStripRearLeft * 100.0f,
	// 	forza.heelOnRumbleStripRearRight * 100.0f);

	// mvprintw(row++, 0, "WheelInPuddleDepthFrontLeft %5.1f FrontRight %5.1f",
	// 	forza.WheelInPuddleDepthFrontLeft * 100.0f,
	// 	forza.WheelInPuddleDepthFrontRight * 100.0f);
	// mvprintw(row++, 0, "WheelInPuddleDepthRearLeft %5.1f RearRight %5.1f",
	// 	forza.WheelInPuddleDepthRearLeft * 100.0f,
	// 	forza.WheelInPuddleDepthRearRight * 100.0f);

	mvprintw(row++, 0, "SurfaceRumbleFrontLeft %5.1f FrontRight %5.1f",
		forza.SurfaceRumbleFrontLeft * 100.0f,
		forza.SurfaceRumbleFrontRight * 100.0f);
	mvprintw(row++, 0, "SurfaceRumbleRearLeft %5.1f RearRight %5.1f",
		forza.SurfaceRumbleRearLeft * 100.0f,
		forza.SurfaceRumbleRearRight * 100.0f);

	mvprintw(row++, 0, "TireSlipAngleFrontLeft %5.1f FrontRight %5.1f",
		forza.TireSlipAngleFrontLeft * 100.0f,
		forza.TireSlipAngleFrontRight * 100.0f);
	mvprintw(row++, 0, "TireSlipAngleRearLeft %5.1f RearRight %5.1f",
		forza.TireSlipAngleRearLeft * 100.0f,
		forza.TireSlipAngleRearRight * 100.0f);

	mvprintw(row++, 0, "TireCombinedSlipFrontLeft %5.1f FrontRight %5.1f",
		forza.TireCombinedSlipFrontLeft * 100.0f,
		forza.TireCombinedSlipFrontRight * 100.0f);
	mvprintw(row++, 0, "TireCombinedSlipRearLeft %5.1f RearRight %5.1f",
		forza.TireCombinedSlipRearLeft * 100.0f,
		forza.TireCombinedSlipRearRight * 100.0f);

	mvprintw(row++, 0, "SuspensionTravelMetersFrontLeft %6.1f cm FrontRight %6.1f cm",
		forza.SuspensionTravelMetersFrontLeft * 1000.0f,
		forza.SuspensionTravelMetersFrontRight * 1000.0f);
	mvprintw(row++, 0, "SuspensionTravelMetersRearLeft %6.1f cm RearRight %6.1f cm",
		forza.SuspensionTravelMetersRearLeft * 1000.0f,
		forza.SuspensionTravelMetersRearRight * 1000.0f);

	mvprintw(row++, 0, "clutch[%3d] brake[%3d] accel[%3d] handbrake[%3d] gear[%2d] Steer[%4d]"
	, forza.Clutch
	, forza.Brake
	, forza.Accel
	, forza.HandBrake
	, forza.Gear
	, forza.Steer
	);

	char drvtype[4][3] =
	{
		"FWD",
		"RWD",
		"AWD"
	};
	mvprintw(row++, 0, "CarOrdinal %5d CarClass %5d CarPerformanceIndex %5d DrivetrainType %4s NumCylinders %5d",
		forza.CarOrdinal, forza.CarClass, forza.CarPerformanceIndex, drvtype[forza.DrivetrainType], forza.NumCylinders);

	mvprintw(row++, 0, "Position %10.5f , %10.5f , %10.5f"
	, forza.PositionX
	, forza.PositionY
	, forza.PositionZ
	);

	mvprintw(row++, 0, "Speed[%6.2f km/h] Power[%7.2f ps] Torque[%5.1f kg.m] Boost[%6.2f] Fuel[%7.2f]"
	, forza.Speed * 3.6 // m/s-> km/h
	, forza.Power / 1000.0f / 0.735 // watt -> kw -> hp
	, forza.Torque / 10.0f
	, forza.Boost
	, forza.Fuel * 100
	);

	// fahrenheit to celsius
	mvprintw(row++, 0, "TireTempFrontLeft %4.1f FrontRight %4.1f",
		(forza.TireTempFrontLeft - 32) * 5 / 9,
		(forza.TireTempFrontRight - 32) * 5 / 9);
	mvprintw(row++, 0, "TireTempRearLeft %4.1f RearRight %4.1f",
		(forza.TireTempRearLeft - 32) * 5 / 9,
		(forza.TireTempRearRight - 32) * 5 / 9);

	// mvprintw(row++, 0, "TireWearFrontLeft %5.1f FrontRight %5.1f",
	// 	forza.TireWearFrontLeft,
	// 	forza.TireWearFrontRight);
	// mvprintw(row++, 0, "TireWearRearLeft %5.1f RearRight %5.1f",
	// 	forza.TireWearRearLeft,
	// 	forza.TireWearRearRight);
#endif

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


