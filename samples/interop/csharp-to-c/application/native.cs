using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace Sample.Interop.Application;

public static partial class Native
{
	[LibraryImport("samples-interop-csharp-to-c-library")]
	[UnmanagedCallConv(CallConvs = new Type[] { typeof(CallConvCdecl) })]
	[DefaultDllImportSearchPaths(DllImportSearchPath.UserDirectories)]
	private static partial IntPtr GetName();

	public static string GetNameValue()
	{
		// Use a raw pointer so the generated code does not free the string literal
		var rawName = GetName();
		return Marshal.PtrToStringAnsi(rawName) ??
			throw new InvalidOperationException("Failed to marshal native string");
	}
}