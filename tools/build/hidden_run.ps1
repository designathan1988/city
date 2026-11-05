# hidden_run.ps1 -Command "<cmd line>" -TimeoutS N: runs the command (through cmd.exe, so redirections work) on a separate
# desktop of the interactive window station (WinSta0\roadcraft_tests): its windows never show on the user's desktop.
# Returns the command's exit code; kills it (by its own process id) on timeout.
param([Parameter(Mandatory = $true)][string]$Command, [int]$TimeoutS = 900, [string]$WorkDir = 'C:\Projects\Roadcraft-windows\fonte')

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class HiddenDesk {
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct STARTUPINFO {
		public int cb; public string lpReserved; public string lpDesktop; public string lpTitle;
		public int dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute, dwFlags;
		public short wShowWindow, cbReserved2; public IntPtr lpReserved2, hStdInput, hStdOutput, hStdError;
	}
	[StructLayout(LayoutKind.Sequential)]
	public struct PROCESS_INFORMATION { public IntPtr hProcess, hThread; public int dwProcessId, dwThreadId; }
	[DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
	public static extern IntPtr CreateDesktop(string name, IntPtr device, IntPtr devmode, int flags, uint access, IntPtr sa);
	[DllImport("user32.dll", SetLastError = true)] public static extern bool CloseDesktop(IntPtr desk);
	[DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
	public static extern bool CreateProcess(string app, string cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags, IntPtr env,
		string dir, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
	[DllImport("kernel32.dll")] public static extern uint WaitForSingleObject(IntPtr h, uint ms);
	[DllImport("kernel32.dll")] public static extern bool GetExitCodeProcess(IntPtr h, out uint code);
	[DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr h);
	public const uint GENERIC_ALL = 0x10000000;
}
'@

$desk = [HiddenDesk]::CreateDesktop('roadcraft_tests', [IntPtr]::Zero, [IntPtr]::Zero, 0, [HiddenDesk]::GENERIC_ALL, [IntPtr]::Zero)
if ($desk -eq [IntPtr]::Zero) { Write-Output "CreateDesktop falhou: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"; exit 98 }
$si = New-Object HiddenDesk+STARTUPINFO
$si.cb = [Runtime.InteropServices.Marshal]::SizeOf($si)
$si.lpDesktop = 'WinSta0\roadcraft_tests'
$pi = New-Object HiddenDesk+PROCESS_INFORMATION
# cmd /S /C "<command>": the outer quotes are stripped and the command keeps its own quotes (cmd /?)
$line = "cmd.exe /s /c `"$Command`""
if (-not [HiddenDesk]::CreateProcess("$env:SystemRoot\System32\cmd.exe", $line, [IntPtr]::Zero, [IntPtr]::Zero, $false, 0, [IntPtr]::Zero, $WorkDir, [ref]$si, [ref]$pi)) {
	Write-Output "CreateProcess falhou: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
	[void][HiddenDesk]::CloseDesktop($desk)
	exit 99
}
$waited = [HiddenDesk]::WaitForSingleObject($pi.hProcess, [uint32]($TimeoutS * 1000))
if ($waited -ne 0) {
	# cmd.exe and what it started: only our own tree, by process id
	& taskkill.exe /PID $pi.dwProcessId /T /F | Out-Null
	Write-Output "tempo-limite: parado o processo $($pi.dwProcessId)"
}
$code = 0
[void][HiddenDesk]::GetExitCodeProcess($pi.hProcess, [ref]$code)
[void][HiddenDesk]::CloseHandle($pi.hProcess)
[void][HiddenDesk]::CloseHandle($pi.hThread)
[void][HiddenDesk]::CloseDesktop($desk)
Write-Output "FIM $code"
exit $code

