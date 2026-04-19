using System;

namespace Sample.Interop.Application;

public class Program
{
	public static void Main(string[] args)
	{
		Console.WriteLine($"Hello World, {Native.GetNameValue()} Style!");
	}
}