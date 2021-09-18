#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#define NUM_APPS 1
#define INFO_SIZE 10
#define MAX_TASKS 15
#define MASTER_ADDR 0x0000
#define BIG_CODE 205
// Application IDs
#define prodcons 0
// Application INFO
static unsigned int appInfo[NUM_APPS][INFO_SIZE] = {
	{ 0x00000000,  // app id prodcons
	  0x00000000,  // app period 
	  0x00000002,  // app executions 
	  0x00000002,  // number of tasks 
	  0xFFFFFFFF,  // nothing 
	  0xFFFFFFFF,  // nothing 
	  0xFFFFFFFF,  // nothing 
	  0xFFFFFFFF,  // nothing 
	  0xFFFFFFFF,  // nothing 
	  0xFFFFFFFF } // nothing 
};

static unsigned int tasksInfo[NUM_APPS][MAX_TASKS][INFO_SIZE] = {
	{ 		 // prodcons
		{ 0x00000000,  // task id prod
		  0x000000CD,  // task size 
		  0x00000081,  // bss size 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF } // nothing 
,
		{ 0x00000001,  // task id cons
		  0x000000B6,  // task size 
		  0x00000081,  // bss size 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF,  // nothing 
		  0xFFFFFFFF } // nothing 
	}
};

static unsigned int tasksCode[NUM_APPS][MAX_TASKS][BIG_CODE] = {
	{ 		 // prodcons
		{ 0xfd010113,  // line 0 - task prod
		  0x02812623,  // line 1
		  0x03010413,  // line 2
		  0xfea42623,  // line 3
		  0xfeb42423,  // line 4
		  0xfec42223,  // line 5
		  0xfed42023,  // line 6
		  0xfce42e23,  // line 7
		  0xfcf42c23,  // line 8
		  0xfd042a23,  // line 9
		  0xfe842503,  // line 10
		  0xfe442583,  // line 11
		  0xfe042603,  // line 12
		  0xfdc42683,  // line 13
		  0xfd842703,  // line 14
		  0xfd442783,  // line 15
		  0xfec42883,  // line 16
		  0x00000073,  // line 17
		  0x00000013,  // line 18
		  0x02c12403,  // line 19
		  0x03010113,  // line 20
		  0x00008067,  // line 21
		  0xfe010113,  // line 22
		  0x00112e23,  // line 23
		  0x00812c23,  // line 24
		  0x02010413,  // line 25
		  0xfea42623,  // line 26
		  0xfeb42423,  // line 27
		  0xfec42583,  // line 28
		  0xfe842603,  // line 29
		  0x00000813,  // line 30
		  0x00000793,  // line 31
		  0x00000713,  // line 32
		  0x00000693,  // line 33
		  0x01e00513,  // line 34
		  0xf75ff0ef,  // line 35
		  0x00100793,  // line 36
		  0x00078513,  // line 37
		  0x01c12083,  // line 38
		  0x01812403,  // line 39
		  0x02010113,  // line 40
		  0x00008067,  // line 41
		  0xfe010113,  // line 42
		  0x00112e23,  // line 43
		  0x00812c23,  // line 44
		  0x02010413,  // line 45
		  0xfea42623,  // line 46
		  0xfeb42423,  // line 47
		  0xfec42583,  // line 48
		  0xfe842603,  // line 49
		  0x00000813,  // line 50
		  0x00000793,  // line 51
		  0x00000713,  // line 52
		  0x00000693,  // line 53
		  0x01400513,  // line 54
		  0xf25ff0ef,  // line 55
		  0x00100793,  // line 56
		  0x00078513,  // line 57
		  0x01c12083,  // line 58
		  0x01812403,  // line 59
		  0x02010113,  // line 60
		  0x00008067,  // line 61
		  0xff010113,  // line 62
		  0x00112623,  // line 63
		  0x00812423,  // line 64
		  0x01010413,  // line 65
		  0x00000813,  // line 66
		  0x00000793,  // line 67
		  0x00000713,  // line 68
		  0x00000693,  // line 69
		  0x00000613,  // line 70
		  0x00000593,  // line 71
		  0x00a00513,  // line 72
		  0xeddff0ef,  // line 73
		  0x00100793,  // line 74
		  0x00078513,  // line 75
		  0x00c12083,  // line 76
		  0x00812403,  // line 77
		  0x01010113,  // line 78
		  0x00008067,  // line 79
		  0xfe010113,  // line 80
		  0x00112e23,  // line 81
		  0x00812c23,  // line 82
		  0x02010413,  // line 83
		  0xfea42623,  // line 84
		  0xfec42583,  // line 85
		  0x00000813,  // line 86
		  0x00000793,  // line 87
		  0x00000713,  // line 88
		  0x00000693,  // line 89
		  0x00000613,  // line 90
		  0x02800513,  // line 91
		  0xe91ff0ef,  // line 92
		  0x00100793,  // line 93
		  0x00078513,  // line 94
		  0x01c12083,  // line 95
		  0x01812403,  // line 96
		  0x02010113,  // line 97
		  0x00008067,  // line 98
		  0xfe010113,  // line 99
		  0x00112e23,  // line 100
		  0x00812c23,  // line 101
		  0x02010413,  // line 102
		  0xfea42623,  // line 103
		  0xfec42583,  // line 104
		  0x00000813,  // line 105
		  0x00000793,  // line 106
		  0x00000713,  // line 107
		  0x00000693,  // line 108
		  0x00000613,  // line 109
		  0x03200513,  // line 110
		  0xe45ff0ef,  // line 111
		  0x00100793,  // line 112
		  0x00078513,  // line 113
		  0x01c12083,  // line 114
		  0x01812403,  // line 115
		  0x02010113,  // line 116
		  0x00008067,  // line 117
		  0xfd010113,  // line 118
		  0x02112623,  // line 119
		  0x02812423,  // line 120
		  0x03010413,  // line 121
		  0xfea42623,  // line 122
		  0xfeb42423,  // line 123
		  0xfec42223,  // line 124
		  0xfed42023,  // line 125
		  0xfce42e23,  // line 126
		  0xfcf42c23,  // line 127
		  0xfec42583,  // line 128
		  0xfe842603,  // line 129
		  0xfe442683,  // line 130
		  0xfe042703,  // line 131
		  0xfdc42783,  // line 132
		  0xfd842503,  // line 133
		  0x00050813,  // line 134
		  0x04200513,  // line 135
		  0xde1ff0ef,  // line 136
		  0x00100793,  // line 137
		  0x00078513,  // line 138
		  0x02c12083,  // line 139
		  0x02812403,  // line 140
		  0x03010113,  // line 141
		  0x00008067,  // line 142
		  0xfe010113,  // line 143
		  0x00112e23,  // line 144
		  0x00812c23,  // line 145
		  0x02010413,  // line 146
		  0x00600793,  // line 147
		  0x00500713,  // line 148
		  0x00400693,  // line 149
		  0x00300613,  // line 150
		  0x00200593,  // line 151
		  0x00100513,  // line 152
		  0xf75ff0ef,  // line 153
		  0xfe042623,  // line 154
		  0x02c0006f,  // line 155
		  0xfec42703,  // line 156
		  0x00000697,  // line 157
		  0x0c46a683,  // line 158
		  0xfec42783,  // line 159
		  0x00279793,  // line 160
		  0x00f687b3,  // line 161
		  0x00e7a223,  // line 162
		  0xfec42783,  // line 163
		  0x00178793,  // line 164
		  0xfef42623,  // line 165
		  0xfec42703,  // line 166
		  0x07f00793,  // line 167
		  0xfce7d8e3,  // line 168
		  0x00000797,  // line 169
		  0x0947a783,  // line 170
		  0x00a00713,  // line 171
		  0x00e7a023,  // line 172
		  0x00000797,  // line 173
		  0x0847a783,  // line 174
		  0x00001737,  // line 175
		  0xb0a70713,  // line 176
		  0x02e7a423,  // line 177
		  0xfe042623,  // line 178
		  0x02c0006f,  // line 179
		  0x00600793,  // line 180
		  0x00500713,  // line 181
		  0x00400693,  // line 182
		  0x00300613,  // line 183
		  0x00200593,  // line 184
		  0x00100513,  // line 185
		  0xef1ff0ef,  // line 186
		  0xfec42783,  // line 187
		  0x00178793,  // line 188
		  0xfef42623,  // line 189
		  0xfec42703,  // line 190
		  0x00900793,  // line 191
		  0xfce7d8e3,  // line 192
		  0x00000793,  // line 193
		  0x00078513,  // line 194
		  0x01c12083,  // line 195
		  0x01812403,  // line 196
		  0x02010113,  // line 197
		  0x00008067,  // line 198
		  0x444f5250,  // line 199
		  0x61745320,  // line 200
		  0x00007472,  // line 201
		  0x444f5250,  // line 202
		  0x646e4520,  // line 203
		  0x00000000 } // line 204
,
		{ 0xfd010113,  // line 0 - task cons
		  0x02812623,  // line 1
		  0x03010413,  // line 2
		  0xfea42623,  // line 3
		  0xfeb42423,  // line 4
		  0xfec42223,  // line 5
		  0xfed42023,  // line 6
		  0xfce42e23,  // line 7
		  0xfcf42c23,  // line 8
		  0xfd042a23,  // line 9
		  0xfe842503,  // line 10
		  0xfe442583,  // line 11
		  0xfe042603,  // line 12
		  0xfdc42683,  // line 13
		  0xfd842703,  // line 14
		  0xfd442783,  // line 15
		  0xfec42883,  // line 16
		  0x00000073,  // line 17
		  0x00000013,  // line 18
		  0x02c12403,  // line 19
		  0x03010113,  // line 20
		  0x00008067,  // line 21
		  0xfe010113,  // line 22
		  0x00112e23,  // line 23
		  0x00812c23,  // line 24
		  0x02010413,  // line 25
		  0xfea42623,  // line 26
		  0xfeb42423,  // line 27
		  0xfec42583,  // line 28
		  0xfe842603,  // line 29
		  0x00000813,  // line 30
		  0x00000793,  // line 31
		  0x00000713,  // line 32
		  0x00000693,  // line 33
		  0x01e00513,  // line 34
		  0xf75ff0ef,  // line 35
		  0x00100793,  // line 36
		  0x00078513,  // line 37
		  0x01c12083,  // line 38
		  0x01812403,  // line 39
		  0x02010113,  // line 40
		  0x00008067,  // line 41
		  0xfe010113,  // line 42
		  0x00112e23,  // line 43
		  0x00812c23,  // line 44
		  0x02010413,  // line 45
		  0xfea42623,  // line 46
		  0xfeb42423,  // line 47
		  0xfec42583,  // line 48
		  0xfe842603,  // line 49
		  0x00000813,  // line 50
		  0x00000793,  // line 51
		  0x00000713,  // line 52
		  0x00000693,  // line 53
		  0x01400513,  // line 54
		  0xf25ff0ef,  // line 55
		  0x00100793,  // line 56
		  0x00078513,  // line 57
		  0x01c12083,  // line 58
		  0x01812403,  // line 59
		  0x02010113,  // line 60
		  0x00008067,  // line 61
		  0xff010113,  // line 62
		  0x00112623,  // line 63
		  0x00812423,  // line 64
		  0x01010413,  // line 65
		  0x00000813,  // line 66
		  0x00000793,  // line 67
		  0x00000713,  // line 68
		  0x00000693,  // line 69
		  0x00000613,  // line 70
		  0x00000593,  // line 71
		  0x00a00513,  // line 72
		  0xeddff0ef,  // line 73
		  0x00100793,  // line 74
		  0x00078513,  // line 75
		  0x00c12083,  // line 76
		  0x00812403,  // line 77
		  0x01010113,  // line 78
		  0x00008067,  // line 79
		  0xfe010113,  // line 80
		  0x00112e23,  // line 81
		  0x00812c23,  // line 82
		  0x02010413,  // line 83
		  0xfea42623,  // line 84
		  0xfec42583,  // line 85
		  0x00000813,  // line 86
		  0x00000793,  // line 87
		  0x00000713,  // line 88
		  0x00000693,  // line 89
		  0x00000613,  // line 90
		  0x02800513,  // line 91
		  0xe91ff0ef,  // line 92
		  0x00100793,  // line 93
		  0x00078513,  // line 94
		  0x01c12083,  // line 95
		  0x01812403,  // line 96
		  0x02010113,  // line 97
		  0x00008067,  // line 98
		  0xfe010113,  // line 99
		  0x00112e23,  // line 100
		  0x00812c23,  // line 101
		  0x02010413,  // line 102
		  0xfea42623,  // line 103
		  0xfec42583,  // line 104
		  0x00000813,  // line 105
		  0x00000793,  // line 106
		  0x00000713,  // line 107
		  0x00000693,  // line 108
		  0x00000613,  // line 109
		  0x03200513,  // line 110
		  0xe45ff0ef,  // line 111
		  0x00100793,  // line 112
		  0x00078513,  // line 113
		  0x01c12083,  // line 114
		  0x01812403,  // line 115
		  0x02010113,  // line 116
		  0x00008067,  // line 117
		  0xfd010113,  // line 118
		  0x02112623,  // line 119
		  0x02812423,  // line 120
		  0x03010413,  // line 121
		  0xfea42623,  // line 122
		  0xfeb42423,  // line 123
		  0xfec42223,  // line 124
		  0xfed42023,  // line 125
		  0xfce42e23,  // line 126
		  0xfcf42c23,  // line 127
		  0xfec42583,  // line 128
		  0xfe842603,  // line 129
		  0xfe442683,  // line 130
		  0xfe042703,  // line 131
		  0xfdc42783,  // line 132
		  0xfd842503,  // line 133
		  0x00050813,  // line 134
		  0x04200513,  // line 135
		  0xde1ff0ef,  // line 136
		  0x00100793,  // line 137
		  0x00078513,  // line 138
		  0x02c12083,  // line 139
		  0x02812403,  // line 140
		  0x03010113,  // line 141
		  0x00008067,  // line 142
		  0xfe010113,  // line 143
		  0x00112e23,  // line 144
		  0x00812c23,  // line 145
		  0x02010413,  // line 146
		  0x00000597,  // line 147
		  0x07458593,  // line 148
		  0x00600793,  // line 149
		  0x00500713,  // line 150
		  0x00400693,  // line 151
		  0x00300613,  // line 152
		  0x00100513,  // line 153
		  0xf71ff0ef,  // line 154
		  0xfe042623,  // line 155
		  0x02c0006f,  // line 156
		  0x00600793,  // line 157
		  0x00500713,  // line 158
		  0x00400693,  // line 159
		  0x00300613,  // line 160
		  0x00200593,  // line 161
		  0x00100513,  // line 162
		  0xf4dff0ef,  // line 163
		  0xfec42783,  // line 164
		  0x00178793,  // line 165
		  0xfef42623,  // line 166
		  0xfec42703,  // line 167
		  0x00900793,  // line 168
		  0xfce7d8e3,  // line 169
		  0x00000793,  // line 170
		  0x00078513,  // line 171
		  0x01c12083,  // line 172
		  0x01812403,  // line 173
		  0x02010113,  // line 174
		  0x00008067,  // line 175
		  0x534e4f43,  // line 176
		  0x61745320,  // line 177
		  0x00007472,  // line 178
		  0x534e4f43,  // line 179
		  0x646e4520,  // line 180
		  0x00000000 } // line 181
	}
};

#endif /*__REPOSITORY_H__*/
