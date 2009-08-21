/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


// logic module cvar definitions
// AX_CVARDECL(name, defaultstring, flags)
AX_CVARDECL(g_fps, "60", Cvar::Cheat);

AX_CVARDECL(net_fps, "20", Cvar::Cheat);

AX_CVARDECL(mouse_sensitivity, "1.0", Cvar::Archive);
AX_CVARDECL(mouse_accel, "0", Cvar::Archive);
AX_CVARDECL(mouse_pitch, "1.0", Cvar::Archive);
AX_CVARDECL(mouse_yaw, "1.0", Cvar::Archive);
AX_CVARDECL(mouse_invert, "0", Cvar::Archive);
AX_CVARDECL(mouse_smooth, "0", Cvar::Archive);
