/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

/*==============================================================================

reference to CPUID instruction specification:
http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
http://download.intel.com/design/Xeon/applnots/24161831.pdf

detecting multi-core on ia32
http://www.intel.com/cd/ids/developer/asmo-na/eng/275339.htm

==============================================================================*/


#include "../private.h"

AX_BEGIN_NAMESPACE

// Copyright (c) 2005 Intel Corporation 
// All Rights Reserved
//
// CPUCount.cpp : Detects three forms of hardware multi-threading support across IA-32 platform
//					The three forms of HW multithreading are: Multi-processor, Multi-core, and 
//					HyperThreading Technology.
//					This application enumerates all the logical processors enabled by OS and BIOS,
//					determine the HW topology of these enabled logical processors in the system 
//					using information provided by CPUID instruction.
//					A multi-processing system can support any combination of the three forms of HW
//					multi-threading support. The relevant topology can be identified using a 
//					three level decomposition of the "initial APIC ID" into 
//					Package_id, core_id, and SMT_id. Such decomposition provides a three-level map of 
//					the topology of hardware resources and
//					allow multi-threaded software to manage shared hardware resources in 
//					the platform to reduce resource contention

//					Multicore detection algorithm for processor and cache topology requires
//					all leaf functions of CPUID instructions be available. System administrator
//					must ensure BIOS settings is not configured to restrict CPUID functionalities.
//-------------------------------------------------------------------------------------------------

#define HWD_MT_BIT         0x10000000     // EDX[28]  Bit 28 is set if HT or multi-core is supported
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with 
                                          // eax set to 1
#define NUM_CORE_BITS      0xFC000000     // EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
                                          // per physical processor when execute cpuid with 
                                          // eax set to 4. 


#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
                                          // initial APIC ID for the processor this code is running on.
                                          


// Status Flag
#define SINGLE_CORE_AND_HT_ENABLED 1
#define SINGLE_CORE_AND_HT_DISABLED 2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE 4
#define MULTI_CORE_AND_HT_NOT_CAPABLE 5
#define MULTI_CORE_AND_HT_ENABLED 6
#define MULTI_CORE_AND_HT_DISABLED 7
#define USER_CONFIG_ISSUE 8


unsigned int  CpuIDSupported(void);
unsigned int  GenuineIntel(void);
unsigned int  HWD_MTSupported(void);
unsigned int  MaxLogicalProcPerPhysicalProc(void);
unsigned int  MaxCorePerPhysicalProc(void);
unsigned int  find_maskwidth(unsigned int);
unsigned char GetAPIC_ID(void);
unsigned char GetNzbSubID(unsigned char,
						  unsigned char,
						  unsigned char);

unsigned char CPUCount(unsigned int *,
					   unsigned int *,
					   unsigned int *);

// Define constant LINUX to compile under Linux
#ifdef __linux__
// 	The Linux source code listing can be compiled using Linux kernel verison 2.6 
//	or higher (e.g. RH 4AS-2.8 using GCC 3.4.4). 
//	Due to syntax variances of Linux affinity APIs with earlier kernel versions 
//	and dependence on glibc library versions, compilation on Linux environment 
//	with older kernels and compilers may require kernel patches or compiler upgrades.

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#define DWORD unsigned long
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include <assert.h>

char g_s3Levels[2048];

#if 0
int main(void) {
   unsigned int  TotAvailLogical   = 0,  // Number of available logical CPU per CORE
				 TotAvailCore  = 0,      // Number of available cores per physical processor
                 PhysicalNum   = 0;      // Total number of physical processors

   unsigned char StatusFlag = 0; 
   int MaxLPPerCore;
   if (CpuIDSupported() < 4) { // CPUID does not report leaf 4 information
	   printf("\nUser Warning:\n CPUID Leaf 4 is not supported or disabled. Please check  \
			 \n BIOS and correct system configuration error if leaf 4 is disabled. \n");
	
   }

   StatusFlag = CPUCount(&TotAvailLogical, &TotAvailCore, &PhysicalNum);
   if (USER_CONFIG_ISSUE == StatusFlag) {
	   printf("User Configuration Error: Not all logical processors in the system are enabled \
			  while running this process. Please rerun this application after make corrections. \n");
	   exit(1);
   }

   printf("\n----Counting Hardware MultiThreading Capabilities and Availability ---------- \n\n");
   printf("This application displays information on three forms of hardware multithreading\n");
   printf("capability and their availability to apps. The three forms of capabilities are:\n");
   printf("multi-processor (MP), Multi-core (core), and HyperThreading Technology (HT).\n");
   printf("\nHardware capability results represents the maximum number provided in hardware.\n");	
   printf("Note, Bios/OS or experienced user can make configuration changes resulting in \n");
   printf("less-than-full HW capabilities are available to applications.\n");
   printf("For best result, the operator is responsible to configure the BIOS/OS such that\n");
   printf("full hardware multi-threading capabilities are enabled.\n");
   printf("\n---------------------------------------------------------------------------- \n\n\n");

   printf("\nCapabilities:\n\n");

   switch (StatusFlag) {
   case MULTI_CORE_AND_HT_NOT_CAPABLE:
	   printf("\tHyper-Threading Technology: not capable  \n\tMulti-core: Yes \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;

   case SINGLE_CORE_AND_HT_NOT_CAPABLE:
	   printf("\tHyper-Threading Technology: Not capable  \n\tMulti-core: No \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;

   case SINGLE_CORE_AND_HT_DISABLED:
	   printf("\tHyper-Threading Technology: Disabled  \n\tMulti-core: No \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;

   case SINGLE_CORE_AND_HT_ENABLED:
	   printf("\tHyper-Threading Technology: Enabled  \n\tMulti-core: No \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;

   case MULTI_CORE_AND_HT_DISABLED:
	   printf("\tHyper-Threading Technology: Disabled  \n\tMulti-core: Yes \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;

   case MULTI_CORE_AND_HT_ENABLED:
	   printf("\tHyper-Threading Technology: Enabled  \n\tMulti-core: Yes \n\tMulti-processor: ");
	   if (PhysicalNum > 1) printf("yes\n"); else printf("No\n");
	   break;
   }



   printf("\n\nHardware capability and its availability to applications: \n");
   printf("\n  System wide availability: %d physical processors, %d cores, %d logical processors\n", \
	       PhysicalNum, TotAvailCore, TotAvailLogical);

   MaxLPPerCore = MaxLogicalProcPerPhysicalProc() / MaxCorePerPhysicalProc() ;
   printf("  Multi-core capabililty : %d cores per package \n", MaxCorePerPhysicalProc());
   printf("  HT capability: %d logical processors per core \n",  MaxLPPerCore);
   assert (PhysicalNum * MaxCorePerPhysicalProc() >= TotAvailCore);
   assert (PhysicalNum * MaxLogicalProcPerPhysicalProc() >= TotAvailLogical);
   if (PhysicalNum * MaxCorePerPhysicalProc() > TotAvailCore)
	   printf("\n  Not all cores in the system are enabled for this application.\n");
   else
	   printf("\n  All cores in the system are enabled for this application.\n");
   
   printf("\n\nRelationships between OS affinity mask, Initial APIC ID, and 3-level sub-IDs: \n");
   printf("\n%s", g_s3Levels);
   printf("\n\nPress Enter To Continue\n");
   getchar();
   return 0;
}
#endif


//
// CpuIDSupported will return 0 if CPUID instruction is unavailable. Otherwise, it will return 
// the maximum supported standard function.
//
unsigned int CpuIDSupported(void) {
	unsigned int MaxInputValue;
// If CPUID instruction is supported
#ifdef __linux__
	try {		
		MaxInputValue = 0;
		// call cpuid with eax = 0
		asm
		(	
			"xorl %%eax,%%eax\n\t"
			"cpuid\n\t"
			: "=a" (MaxInputValue)
			: 
			: "%ebx", "%ecx", "%edx"
		);		
	} catch(...) {
		return(0);                   // cpuid instruction is unavailable
	}
#else //Win32
	try {
		MaxInputValue = 0;
		// call cpuid with eax = 0
		__asm {
			xor eax, eax
			cpuid
			mov MaxInputValue, eax
		}
	} catch (...) {
		return(0);                   // cpuid instruction is unavailable
	}
#endif

	return MaxInputValue;

}


//
// GenuineIntel will return 0 if the processor is not a Genuine Intel Processor 
//
unsigned int GenuineIntel(void)
{
#ifdef __linux__
	unsigned int VendorIDb = 0,VendorIDd = 0, VendorIDc = 0;
		
	try    
    // If CPUID instruction is supported
	{
		// Get vendor id string
		asm (
	   		//get the vendor string
			// call cpuid with eax = 0
			"xorl %%eax, %%eax\n\t"			
			"cpuid\n\t"
			: 	"=b" (VendorIDb),
				"=d" (VendorIDd),
				"=c" (VendorIDc)
			:
			: "%eax"
		);
	} catch(...) {
		return(0);                   // cpuid instruction is unavailable
	}

	return ((VendorIDb == AX_MAKEFOURCC('n', 'n', 'e', 'G') /*'uneG'*/) &&
			 (VendorIDd == AX_MAKEFOURCC('I', 'e', 'e', 'i') /*'Ieni'*/) &&
			 (VendorIDc == AX_MAKEFOURCC('l', 'e', 't', 'n') /*'letn'*/)); 
	
#else
	unsigned int VendorID[3] = {0, 0, 0};
	try {   // If CPUID instruction is supported
		__asm {
			xor eax, eax // call cpuid with eax = 0
        	cpuid // Get vendor id string
			mov VendorID, ebx
			mov VendorID + 4, edx
			mov VendorID + 8, ecx
		}
	} catch (...) {
		return(0);      		unsigned int MaxInputValue =0;
		             // cpuid instruction is unavailable
	}
	return ((VendorID[0] == 'uneG') &&
			 (VendorID[1] == 'Ieni') &&
			 (VendorID[2] == 'letn')); 
#endif
}



//
// Function returns the maximum cores per physical package. Note that the number of 
// AVAILABLE cores per physical to be used by an application might be less than this
// maximum value.
//

unsigned int MaxCorePerPhysicalProc(void) {
	unsigned int Regeax        = 0;
	
	if (!HWD_MTSupported()) return (unsigned int) 1;  // Single core
#ifdef __linux__
	{
		asm (
			"xorl %eax, %eax\n\t"
			"cpuid\n\t"
			"cmpl $4, %eax\n\t"			// check if cpuid supports leaf 4
			"jl .single_core\n\t"		// Single core
			"movl $4, %eax\n\t"		
			"movl $0, %ecx\n\t"			// start with index = 0; Leaf 4 reports
		);								// at least one valid cache level
		asm (
			"cpuid"
			: "=a" (Regeax)
			:
			: "%ebx", "%ecx", "%edx"
		);		
		asm (
			"jmp .multi_core\n"
			".single_core:\n\t"
			"xor %eax, %eax\n"
			".multi_core:"
		);		
	}
#else
		__asm {
			xor eax, eax
			cpuid
			cmp eax, 4 // check if cpuid supports leaf 4
			jl single_core // Single core
			mov eax, 4 
			mov ecx, 0 // start with index = 0; Leaf 4 reports
			cpuid // at least one valid cache level
			mov Regeax, eax
			jmp multi_core

single_core:
			xor eax, eax 

multi_core:
		}
#endif
	return (unsigned int)((Regeax & NUM_CORE_BITS) >> 26)+1;

}





//
// The function returns 0 when the hardware multi-threaded bit is not set.
//
unsigned int HWD_MTSupported(void) {
	unsigned int Regedx      = 0;

	if ((CpuIDSupported() >= 1) && GenuineIntel()) {
#ifdef __linux__
		asm (
			"movl $1,%%eax\n\t"
			"cpuid"
			: "=d" (Regedx)
			:
			: "%eax","%ebx","%ecx"
		);
#else
		__asm {
			mov eax, 1
			cpuid
			mov Regedx, edx
		}		
#endif
	}

	return (Regedx & HWD_MT_BIT);  
}

//
// Function returns the maximum logical processors per physical package. Note that the number of 
// AVAILABLE logical processors per physical to be used by an application might be less than this
// maximum value.
//
unsigned int MaxLogicalProcPerPhysicalProc(void) {

	unsigned int Regebx = 0;

	if (!HWD_MTSupported()) return (unsigned int) 1;
#ifdef __linux__
		asm (
			"movl $1,%%eax\n\t"
			"cpuid"
			: "=b" (Regebx)
			:
			: "%eax","%ecx","%edx"
		);
#else
		__asm {
			mov eax, 1
			cpuid
			mov Regebx, ebx
		}
#endif
		return (unsigned int) ((Regebx & NUM_LOGICAL_BITS) >> 16);

}


unsigned char GetAPIC_ID(void)
{

	unsigned int Regebx = 0;
#ifdef __linux__
	asm (
		"movl $1, %%eax\n\t"	
		"cpuid"
		: "=b" (Regebx) 
		:
		: "%eax","%ecx","%edx" 
	);
	
#else
	__asm {
		mov eax, 1
		cpuid
		mov Regebx, ebx
	}
#endif                                

	return (unsigned char) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);

}


//
// Determine the width of the bit field that can represent the value count_item. 
//
unsigned int find_maskwidth(unsigned int CountItem)
{
	unsigned int MaskWidth,
				 count = CountItem;
#ifdef __linux__
	asm
	(
#ifdef __x86_64__ // define constant to compile  
		"push %%rcx\n\t"		// under 64-bit Linux
		"push %%rax\n\t"
#else
		"pushl %%ecx\n\t"
		"pushl %%eax\n\t"
#endif
//		"movl $count, %%eax\n\t" //done by Assembler below
		"xorl %%ecx, %%ecx"
//		"movl %%ecx, MaskWidth\n\t" //done by Assembler below
		: "=c" (MaskWidth)
		: "a" (count)
//		: "%ecx", "%eax" We don't list these as clobbered because we don't want the assembler
			//to put them back when we are done
	);
	asm
	(
		"decl %%eax\n\t"
		"bsrw %%ax,%%cx\n\t"
		"jz next\n\t"
		"incw %%cx\n\t"
//		"movl %%ecx, MaskWidth\n" //done by Assembler below
		: "=c" (MaskWidth)
		:
	);
	asm
	(
		"next:\n\t"
#ifdef __x86_64__
		"pop %rax\n\t"
		"pop %rcx"		
#else
		"popl %eax\n\t"
		"popl %ecx"		
#endif
	);

#else
	__asm
	{
		mov eax, count
		mov ecx, 0
		mov MaskWidth, ecx
		dec eax
		bsr cx, ax
		jz next
		inc cx
		mov MaskWidth, ecx
next:
		
	}
#endif
	return MaskWidth;
}


//
// Extract the subset of bit field from the 8-bit value FullID.  It returns the 8-bit sub ID value
//
unsigned char GetNzbSubID(unsigned char FullID,
						  unsigned char MaxSubIDValue,
						  unsigned char ShiftCount)
{
	unsigned int MaskWidth;
	unsigned char MaskBits;

	MaskWidth = find_maskwidth((unsigned int) MaxSubIDValue);
	MaskBits  = (0xff << ShiftCount) ^ 
				((unsigned char) (0xff << (ShiftCount + MaskWidth)));

	return (FullID & MaskBits);
}




//
//
//
unsigned char CPUCount(unsigned int *TotAvailLogical,
					   unsigned int *TotAvailCore,
					   unsigned int *PhysicalNum)
{
	unsigned char StatusFlag = 0;
	unsigned int numLPEnabled = 0;
	DWORD dwAffinityMask;
	int j = 0, MaxLPPerCore;
	unsigned char apicID, PackageIDMask;
	unsigned char tblPkgID[256], tblCoreID[256], tblSMTID[256];
	char tmp[256];
	g_s3Levels[0] = 0;
	*TotAvailCore = 1;
	*PhysicalNum  = 1;

#ifdef __linux__
	//we need to make sure that this process is allowed to run on 
	//all of the logical processors that the OS itself can run on.
	//A process could acquire/inherit affinity settings that restricts the 
	// current process to run on a subset of all logical processor visible to OS.

	// Linux doesn't easily allow us to look at the Affinity Bitmask directly,
	// but it does provide an API to test affinity maskbits of the current process 
	// against each logical processor visible under OS.
	int sysNumProcs = sysconf(_SC_NPROCESSORS_CONF); //This will tell us how many 
													//CPUs are currently enabled.
	
	//this will tell us which processors this process can run on. 
	 cpu_set_t allowedCPUs;	 
	 sched_getaffinity(0, sizeof(allowedCPUs), &allowedCPUs);
	 
	 for (int i = 0; i < sysNumProcs; i++)
	{
		if (CPU_ISSET(i, &allowedCPUs) == 0)
		{
			StatusFlag = USER_CONFIG_ISSUE;		
			return StatusFlag;
		}
	}
#else
	DWORD dwProcessAffinity, dwSystemAffinity;
	GetProcessAffinityMask(GetCurrentProcess(), 
						   &dwProcessAffinity,
						   &dwSystemAffinity);
	if (dwProcessAffinity != dwSystemAffinity)  // not all CPUs are enabled
	{
		StatusFlag = USER_CONFIG_ISSUE;		
		return StatusFlag;
	}
#endif
	
	// Assumwe that cores within a package have the SAME number of 
	// logical processors.  Also, values returned by
	// MaxLogicalProcPerPhysicalProc and MaxCorePerPhysicalProc do not have
	// to be power of 2.

	MaxLPPerCore = MaxLogicalProcPerPhysicalProc() / MaxCorePerPhysicalProc();
	dwAffinityMask = 1;

#ifdef __linux__
	cpu_set_t currentCPU;
	while (j < sysNumProcs)
	{
		CPU_ZERO(&currentCPU);
		CPU_SET(j, &currentCPU);
		if (sched_setaffinity (0, sizeof(currentCPU), &currentCPU) == 0)
		{
			sleep(0);  // Ensure system to switch to the right CPU
#else
	while (dwAffinityMask && dwAffinityMask <= dwSystemAffinity)
	{
		if (SetThreadAffinityMask(GetCurrentThread(), dwAffinityMask))
		{
			Sleep(0);  // Ensure system to switch to the right CPU
#endif
			apicID = GetAPIC_ID();


			// Store SMT ID and core ID of each logical processor
			// Shift vlaue for SMT ID is 0
			// Shift value for core ID is the mask width for maximum logical
			// processors per core

			tblSMTID[j]  = GetNzbSubID(apicID, MaxLPPerCore, 0);
			tblCoreID[j] = GetNzbSubID(apicID, 
						   MaxCorePerPhysicalProc(),
						   (unsigned char) find_maskwidth(MaxLPPerCore));

			// Extract package ID, assume single cluster.
			// Shift value is the mask width for max Logical per package

			PackageIDMask = (unsigned char) (0xff << 
							find_maskwidth(MaxLogicalProcPerPhysicalProc()));

			tblPkgID[j] = apicID & PackageIDMask;
			sprintf(tmp,"  AffinityMask = %d; Initial APIC = %d; Physical ID = %d, Core ID = %d,  SMT ID = %d\n",
				    dwAffinityMask, apicID, tblPkgID[j], tblCoreID[j], tblSMTID[j]);
			strcat(g_s3Levels, tmp);

			numLPEnabled ++;   // Number of available logical processors in the system.

		} // if

		j++;
		dwAffinityMask = 1 << j;
	} // while

    // restore the affinity setting to its original state
#ifdef __linux__
	sched_setaffinity (0, sizeof(allowedCPUs), &allowedCPUs);
	sleep(0);
#else
    SetThreadAffinityMask(GetCurrentThread(), dwProcessAffinity);
	Sleep(0);
#endif
	*TotAvailLogical = numLPEnabled;
	
	//
	// Count available cores (TotAvailCore) in the system
	//
	unsigned char CoreIDBucket[256];
	DWORD ProcessorMask, pCoreMask[256];
	unsigned int i, ProcessorNum;

	CoreIDBucket[0] = tblPkgID[0] | tblCoreID[0];
	ProcessorMask = 1;
	pCoreMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *TotAvailCore; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if ((tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum]) == CoreIDBucket[i])
			{
				pCoreMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *TotAvailCore)   // did not match any bucket.  Start a new one.
		{
			CoreIDBucket[i] = tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum];
			pCoreMask[i] = ProcessorMask;

			(*TotAvailCore)++;	// Number of available cores in the system

		}

	}  // for ProcessorNum


	//
	// Count physical processor (PhysicalNum) in the system
	//
	unsigned char PackageIDBucket[256];
	DWORD pPackageMask[256];

	PackageIDBucket[0] = tblPkgID[0];
	ProcessorMask = 1;
	pPackageMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *PhysicalNum; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if (tblPkgID[ProcessorNum]== PackageIDBucket[i])
			{
				pPackageMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *PhysicalNum)   // did not match any bucket.  Start a new one.
		{
			PackageIDBucket[i] = tblPkgID[ProcessorNum];
			pPackageMask[i] = ProcessorMask;

			(*PhysicalNum)++;	// Total number of physical processors in the system

		}

	}  // for ProcessorNum

	//
	// Check to see if the system is multi-core 
	// Check if the system is hyper-threading
	//
	if (*TotAvailCore > *PhysicalNum) 
	{
		// Multi-core
		if (MaxLPPerCore == 1)
			StatusFlag = MULTI_CORE_AND_HT_NOT_CAPABLE;
		else if (numLPEnabled > *TotAvailCore)
			StatusFlag = MULTI_CORE_AND_HT_ENABLED;
		else StatusFlag = MULTI_CORE_AND_HT_DISABLED;

	}
	else
	{
		// Single-core
		if (MaxLPPerCore == 1)
			StatusFlag = SINGLE_CORE_AND_HT_NOT_CAPABLE;
		else if (numLPEnabled > *TotAvailCore)
			StatusFlag = SINGLE_CORE_AND_HT_ENABLED;
		else StatusFlag = SINGLE_CORE_AND_HT_DISABLED;


	}

	return StatusFlag;
}


/* features */
#define FPU_FLAG 0x0001
#define MMX_FLAG 0x800000 // 1 << 23
#define ISSE_FLAG 0x2000000 // 1 << 25
#define ISSE2_FLAG 0x4000000 // 1 << 26
#define HTT_FLAG (1<<28)

#define ISSE3_FLAG (1<<0)		// ECX

#define A3DNOW_FLAG 0x80000000 // 1<<31
#define A3DNOWPLUS_FLAG 0x40000000 // 1<<30
#define MMXPLUS_FLAG 0x1000000 // 1<<24


void DetectCpuInfo(CpuInfo& cpu_info) {
#ifdef _MSC_VER
	const unsigned char hex_chars[16] =
	    { '0', '1', '2', '3', '4', '5', '6', '7',
	      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};
	uint_t signature = 0;
	uint_t cache_eax;
	uint_t cache_ebx;
	uint_t cache_ecx;
	uint_t cache_edx;
	uint_t features_ebx = 0;
	uint_t features_ecx = 0;
	uint_t features_edx = 0;
	uint_t features_edx_2 = 0;
	ushort_t fp_status;
	int family;
	int fpu_type;
	int celeron_flag;
	int pentiumxeon_flag;
	int name_flag;
	char vendor[13];
	char name[49];
	char *cpu_string;
	char *vendor_string;
	float test_val[4] = { 1.f, 1.f, 1.f, 1.f };

	celeron_flag = 0;
	pentiumxeon_flag = 0;
	name_flag = 0;

	__asm {
		/*
		 * Execute CPUID instruction to determine vendor, family,
		 * model, stepping and features.
		 */
		push ebx /* save registers */
		push esi
		push edi
		xor eax, eax /* set up for CPUID instruction */
		cpuid /* get and save vendor ID */
		mov dword ptr[vendor+0], ebx
		mov dword ptr[vendor+4], edx
		mov dword ptr[vendor+8], ecx
		mov byte ptr[vendor+12], 0
		cmp eax, 1 /* make sure 1 is valid input for CPUID */
		jl end_CPUID_type /* if not, jump to end */
		mov eax, 1
		cpuid /* get family/model/stepping/features */
		mov signature, eax
		mov features_ebx, ebx
		mov features_ecx, ecx
		mov features_edx, edx
		shr eax, 8 /* isolate family */
		and eax, 0fh
		mov family, eax /* set cpu_type with family */

		/*
		 * Execute CPUID instruction to determine the cache descriptor
		 * information.
		 */
		xor eax, eax /* set up to check the EAX value */
		cpuid
		cmp ax, 2 /* are cache descriptors supported? */
		jl end_CPUID_type
		mov eax, 2 /* set up to read cache descriptor */
		cpuid
		cmp al, 1 /* is one iteration enough to obtain */
		jne end_CPUID_type /* cache information? */

		/* this code supports one iteration only. */
		mov cache_eax, eax /* store cache information */
		mov cache_ebx, ebx /* NOTE: for future processors, CPUID */
		mov cache_ecx, ecx /* instruction may need to be run more */
		mov cache_edx, edx /* than once to get complete cache information */
end_CPUID_type:
		pop edi /* restore registers */
		pop esi
		pop ebx
		
		/* check for 3DNow! */
		mov eax, 080000000h /* query for extended functions */
		cpuid /* get extended function limit */
		cmp eax, 080000001h /* functions up to 80000001h must be present */
		jb no_extended /* 80000001h is not available */
		mov eax, 080000001h /* setup extended function 1 */
		cpuid /* call the function */
		mov features_edx_2, edx

		/* get CPU name */
		mov eax, 080000000h
		cpuid
		cmp eax, 080000004h
		jb end_name /* functions up to 80000004h must be present */
		mov name_flag, 1
		mov eax, 080000002h
		cpuid
		mov dword ptr[name], eax
		mov dword ptr[name + 4], ebx
		mov dword ptr[name + 8], ecx
		mov dword ptr[name + 12], edx
		mov eax, 080000003h
		cpuid
		mov dword ptr[name + 16], eax
		mov dword ptr[name + 20], ebx
		mov dword ptr[name + 24], ecx
		mov dword ptr[name + 28], edx
		mov eax, 080000004h
		cpuid
		mov dword ptr[name + 32], eax
		mov dword ptr[name + 36], ebx
		mov dword ptr[name + 40], ecx
		mov dword ptr[name + 44], edx
		mov byte ptr[name + 48], 0
end_name:

no_extended:
		/* detect FPU */
		fninit /* reset FP status word */
		mov fp_status, 05a5ah /* initialize temp word to non-zero */
		fnstsw fp_status /* save FP status word */
		mov ax, fp_status /* check FP status word */
		cmp al, 0 /* was correct status written */
		mov fpu_type, 0 /* no FPU present */
		jne end_fpu_type
		/* check control word */
		fnstcw fp_status /* save FP control word */
		mov ax, fp_status /* check FP control word */
		and ax, 103fh /* selected parts to examine */
		cmp ax, 3fh /* was control word correct */
		mov fpu_type, 0
		jne end_fpu_type /* incorrect control word, no FPU */
		mov fpu_type, 1
end_fpu_type:
	}

	cpu_info.cpuid = signature;
	cpu_info.family = (signature >> 8) & 0xF;
	cpu_info.model = (signature >> 4) & 0xF;
	cpu_info.stepping = signature & 0xF;

	cpu_info.features = 0;
	if (features_edx_2 & A3DNOW_FLAG && features_edx_2 & A3DNOWPLUS_FLAG)
		cpu_info.features |= PF_3DNOW;
	if (features_edx & MMX_FLAG)
		cpu_info.features |= PF_MMX;
#if 0
	if (features_edx_2 & MMXPLUS_FLAG)
		cpu_info.features |= PF_MMXPLUS;
#endif
	if (features_edx & ISSE_FLAG)
		cpu_info.features |= PF_SSE;
	if (features_edx & ISSE2_FLAG)
		cpu_info.features |= PF_SSE2;
	if (features_edx & HTT_FLAG)
		cpu_info.features |= PF_HTT;
	if (features_ecx & ISSE3_FLAG)
		cpu_info.features |= PF_SSE3;


	vendor_string = "unknown";
	cpu_string = "unknown";

	/* using CPUID instruction */
	if (name_flag) {
		vendor_string = vendor;
		cpu_string = name;
	}

	cpu_info.cpu_type = l2u(cpu_string);
	cpu_info.vendor = l2u(vendor_string);
	cpu_info.numLogicCores = 1;

	unsigned int  TotAvailLogical   = 0,  // Number of available logical CPU per CORE
		TotAvailCore  = 0,      // Number of available cores per physical processor
		PhysicalNum   = 0;      // Total number of physical processors

	unsigned char StatusFlag = 0; 
	if (CpuIDSupported() < 4) { // CPUID does not report leaf 4 information
		printf("\nUser Warning:\n CPUID Leaf 4 is not supported or disabled. Please check  \
			   \n BIOS and correct system configuration error if leaf 4 is disabled. \n");

	}

	StatusFlag = CPUCount(&TotAvailLogical, &TotAvailCore, &PhysicalNum);
	if (USER_CONFIG_ISSUE == StatusFlag) {
		printf("User Configuration Error: Not all logical processors in the system are enabled \
			   while running this process. Please rerun this application after make corrections. \n");
		return;
	}

	cpu_info.numLogicCores = TotAvailLogical;
#endif
}

AX_END_NAMESPACE
