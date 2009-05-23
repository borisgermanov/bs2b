/*
 * LADSPA bs2b effect plugin
 * Copyright (C) 2009, Sebastian Pipping <sebastian@pipping.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* delay.c

   Free software by Richard W.E. Furse. Do with as you will. No
   warranty.

   This LADSPA plugin provides a simple delay line implemented in
   C. There is a fixed maximum delay length and no feedback is
   provided.

   This file has poor memory protection. Failures during malloc() will
   not recover nicely.
*/

#include <config.h>
#include <ladspa.h>
#include <bs2b/bs2b.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LB_BETWEEN(min, x, max) (((x) < (min))\
	? (min)\
	: (\
		((x) > (max))\
		? (max)\
		: (x)\
	)\
)

/*****************************************************************************/

/* The port numbers for the plugin: */
enum LadspaBs2bConstants {
	LB_PORT_LOWPASS,
	LB_PORT_FEEDING,
	LB_PORT_INPUT_LEFT,
	LB_PORT_INPUT_RIGHT,
	LB_PORT_OUTPUT_LEFT,
	LB_PORT_OUTPUT_RIGHT,

	LB_PORT_COUNT
};

/*****************************************************************************/

/* Instance data for the bs2b plugin. */
typedef struct {
	t_bs2bdp bs2b;
	uint32_t levelBackup;

	/* Ports from here on */
	LADSPA_Data * m_pfLowpass;
	LADSPA_Data * m_pfFeeding;

	/* Input audio port data location. */
	LADSPA_Data * m_pfInputLeft;
	LADSPA_Data * m_pfInputRight;

	/* Output audio port data location. */
	LADSPA_Data * m_pfOutputLeft;
	LADSPA_Data * m_pfOutputRight;
} Bs2bLine;

/*****************************************************************************/

/* Construct a new plugin instance. */
LADSPA_Handle
instantiateBs2bLine(const LADSPA_Descriptor * Descriptor,
		unsigned long SampleRate) {
	Bs2bLine * psBs2bLine = (Bs2bLine *)malloc(sizeof(Bs2bLine));
	if (psBs2bLine == NULL) {
		return NULL;
	}

	/* Sample rate supported? */
	if ((SampleRate < BS2B_MINSRATE) || (SampleRate > BS2B_MAXSRATE)) {
		return NULL;
	}

	/* Init effect backend */
	psBs2bLine->bs2b = bs2b_open();
	if(psBs2bLine->bs2b == NULL) {
		free(psBs2bLine);
		return NULL;
	}
	bs2b_set_srate(psBs2bLine->bs2b, SampleRate);
	psBs2bLine->levelBackup = BS2B_DEFAULT_CLEVEL;

	return psBs2bLine;
}

/*****************************************************************************/

/* Initialise and activate a plugin instance. */
void
activateBs2bLine(LADSPA_Handle Instance) {
	/*
	Bs2bLine * psBs2bLine = (Bs2bLine *)Instance;
	NOOP
	*/
}

/*****************************************************************************/

/* Connect a port to a data location. */
void
connectPortToBs2bLine(LADSPA_Handle Instance, unsigned long Port,
		LADSPA_Data * DataLocation) {
	Bs2bLine * psBs2bLine = (Bs2bLine *)Instance;
	switch (Port) {
	case LB_PORT_LOWPASS:
		psBs2bLine->m_pfLowpass = DataLocation;
		break;
	case LB_PORT_FEEDING:
		psBs2bLine->m_pfFeeding = DataLocation;
		break;
	case LB_PORT_INPUT_LEFT:
		psBs2bLine->m_pfInputLeft = DataLocation;
		break;
	case LB_PORT_INPUT_RIGHT:
		psBs2bLine->m_pfInputRight = DataLocation;
		break;
	case LB_PORT_OUTPUT_LEFT:
		psBs2bLine->m_pfOutputLeft = DataLocation;
		break;
	case LB_PORT_OUTPUT_RIGHT:
		psBs2bLine->m_pfOutputRight = DataLocation;
		break;
	}
}

/*****************************************************************************/

/* Run a bs2b instance for a block of SampleCount samples. */
void
runBs2bLine(LADSPA_Handle Instance,
		   unsigned long SampleCount) {
	/* TODO optimize re-allocation away */
	float * const alternating = malloc(sizeof(float) * SampleCount * 2);

	LADSPA_Data * pfInputLeft;
	LADSPA_Data * pfInputRight;
	LADSPA_Data * pfOutputLeft;
	LADSPA_Data * pfOutputRight;
	Bs2bLine * psBs2bLine = (Bs2bLine *)Instance;
	unsigned long lSampleIndex;

	uint16_t const lowpass = (uint16_t)psBs2bLine->m_pfLowpass[0];
	uint16_t const feeding = (uint16_t)(psBs2bLine->m_pfFeeding[0] * 10);
	uint32_t const currentLevel
		= LB_BETWEEN(BS2B_MINFCUT, lowpass, BS2B_MAXFCUT)
		| LB_BETWEEN(BS2B_MINFEED, feeding, BS2B_MAXFEED) << 16;

	pfInputLeft = psBs2bLine->m_pfInputLeft;
	pfInputRight = psBs2bLine->m_pfInputRight;
	pfOutputLeft = psBs2bLine->m_pfOutputLeft;
	pfOutputRight = psBs2bLine->m_pfOutputRight;

	/* Write to channel-alternating buffer */
	for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++) {
		alternating[2 * lSampleIndex + 0] = pfInputLeft[lSampleIndex];
		alternating[2 * lSampleIndex + 1] = pfInputRight[lSampleIndex];
	}

	/* Sync settings */
	if (currentLevel != psBs2bLine->levelBackup) {
		bs2b_set_level(psBs2bLine->bs2b, currentLevel);
		psBs2bLine->levelBackup = currentLevel;
	}

	/* Apply effect */
	bs2b_cross_feed_f(psBs2bLine->bs2b, alternating, SampleCount);

	/* Read back from channel-alternating buffer */
	for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++) {
		pfOutputLeft[lSampleIndex] = alternating[2 * lSampleIndex + 0];
		pfOutputRight[lSampleIndex] = alternating[2 * lSampleIndex + 1];
	}
}

/*****************************************************************************/

/* Throw away a bs2b line. */
void
cleanupBs2bLine(LADSPA_Handle Instance) {
	Bs2bLine * psBs2bLine = (Bs2bLine *)Instance;
	if (psBs2bLine->bs2b != NULL) {
		bs2b_close(psBs2bLine->bs2b);
		psBs2bLine->bs2b = NULL;
	}
	free(psBs2bLine);
}

/*****************************************************************************/

LADSPA_Descriptor * g_psDescriptor = NULL;

/*****************************************************************************/

/* _init() is called automatically when the plugin library is first
   loaded. */
void
_init() {
	char ** pcPortNames;
	LADSPA_PortDescriptor * piPortDescriptors;
	LADSPA_PortRangeHint * psPortRangeHints;

	g_psDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
	if (g_psDescriptor) {
		/* TODO replace with registered uniqui bs2b ladspa id */
		g_psDescriptor->UniqueID = 1043;
		g_psDescriptor->Label = strdup("bs2b");
		g_psDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		g_psDescriptor->Name = strdup("Bauer stereophonic-to-binaural " VERSION);

		{
			const char * const version_format
				= "Bauer stereophonic-to-binaural " VERSION " (%s)";
			const int formatLen = strlen(version_format);
			const int bs2bVersionLen = strlen(bs2b_runtime_version());
			const int finalLen = formatLen - 2 + bs2bVersionLen;

			char * const finalName = malloc(finalLen + 1);
			if (finalName != NULL) {
				snprintf(finalName, finalLen + 1, version_format,
						bs2b_runtime_version());
				finalName[finalLen] = '\0';

				g_psDescriptor->Name = finalName;
			}
		}

		g_psDescriptor->Maker = strdup("Boris Mikhaylov, Sebastian Pipping");
		g_psDescriptor->Copyright = strdup("GPL 2 or later");
		g_psDescriptor->PortCount = LB_PORT_COUNT;
		piPortDescriptors = (LADSPA_PortDescriptor *)
			calloc(LB_PORT_COUNT, sizeof(LADSPA_PortDescriptor));
		g_psDescriptor->PortDescriptors
			= (const LADSPA_PortDescriptor *)piPortDescriptors;
		piPortDescriptors[LB_PORT_LOWPASS]
			= LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		piPortDescriptors[LB_PORT_FEEDING]
			= LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		piPortDescriptors[LB_PORT_INPUT_LEFT]
			= LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		piPortDescriptors[LB_PORT_INPUT_RIGHT]
			= LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		piPortDescriptors[LB_PORT_OUTPUT_LEFT]
			= LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		piPortDescriptors[LB_PORT_OUTPUT_RIGHT]
			= LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		pcPortNames = (char **)calloc(LB_PORT_COUNT, sizeof(char *));
		g_psDescriptor->PortNames = (const char **)pcPortNames;
		pcPortNames[LB_PORT_LOWPASS]
			= strdup("Lowpass filter cut frequency (Hz)");
		pcPortNames[LB_PORT_FEEDING] = strdup("Feeding level (dB)");
		pcPortNames[LB_PORT_INPUT_LEFT] = strdup("Input left");
		pcPortNames[LB_PORT_INPUT_RIGHT] = strdup("Input right");
		pcPortNames[LB_PORT_OUTPUT_LEFT] = strdup("Output left");
		pcPortNames[LB_PORT_OUTPUT_RIGHT] = strdup("Output right");
		psPortRangeHints = ((LADSPA_PortRangeHint *)
			calloc(LB_PORT_COUNT, sizeof(LADSPA_PortRangeHint)));
		g_psDescriptor->PortRangeHints
			= (const LADSPA_PortRangeHint *)psPortRangeHints;

		psPortRangeHints[LB_PORT_LOWPASS].HintDescriptor
			= (LADSPA_HINT_BOUNDED_BELOW
			| LADSPA_HINT_BOUNDED_ABOVE
			| LADSPA_HINT_DEFAULT_MIDDLE);
		psPortRangeHints[LB_PORT_LOWPASS].LowerBound = 400;
		psPortRangeHints[LB_PORT_LOWPASS].UpperBound = 1000;

		psPortRangeHints[LB_PORT_FEEDING].HintDescriptor
			= (LADSPA_HINT_BOUNDED_BELOW
			| LADSPA_HINT_BOUNDED_ABOVE
			| LADSPA_HINT_DEFAULT_MIDDLE);
		psPortRangeHints[LB_PORT_FEEDING].LowerBound = 1; /* i.e. 10 */
		psPortRangeHints[LB_PORT_FEEDING].UpperBound = 11; /* i.e. 110 */

		psPortRangeHints[LB_PORT_INPUT_LEFT].HintDescriptor = 0;
		psPortRangeHints[LB_PORT_INPUT_RIGHT].HintDescriptor = 0;
		psPortRangeHints[LB_PORT_OUTPUT_LEFT].HintDescriptor = 0;
		psPortRangeHints[LB_PORT_OUTPUT_RIGHT].HintDescriptor = 0;
		g_psDescriptor->instantiate = instantiateBs2bLine;
		g_psDescriptor->connect_port = connectPortToBs2bLine;
		g_psDescriptor->activate = activateBs2bLine;
		g_psDescriptor->run = runBs2bLine;
		g_psDescriptor->run_adding = NULL;
		g_psDescriptor->set_run_adding_gain = NULL;
		g_psDescriptor->deactivate = NULL;
		g_psDescriptor->cleanup = cleanupBs2bLine;
	}
}

/*****************************************************************************/

/* _fini() is called automatically when the library is unloaded. */
void
_fini() {
	long lIndex;
	if (g_psDescriptor) {
		free((char *)g_psDescriptor->Label);
		free((char *)g_psDescriptor->Name);
		free((char *)g_psDescriptor->Maker);
		free((char *)g_psDescriptor->Copyright);
		free((LADSPA_PortDescriptor *)g_psDescriptor->PortDescriptors);
		for (lIndex = 0; lIndex < g_psDescriptor->PortCount; lIndex++) {
			free((char *)(g_psDescriptor->PortNames[lIndex]));
		}
		free((char **)g_psDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)g_psDescriptor->PortRangeHints);
		free(g_psDescriptor);
	}
}

/*****************************************************************************/

/* Return a descriptor of the requested plugin type. Only one plugin
   type is available in this library. */
const LADSPA_Descriptor * 
ladspa_descriptor(unsigned long Index) {
	if (Index == 0)
		return g_psDescriptor;
	else
		return NULL;
}

/*****************************************************************************/

/* EOF */
