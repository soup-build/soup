﻿// <copyright file="RuntimeProcess.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Opal.System
{
    using global::System;
    using global::System.Diagnostics;

    /// <summary>
    /// The shared runtime process executable using system.
    /// </summary>
    public class RuntimeProcess : IProcess
    {
        // Input
        private Path executable;
        private string arguments;
        private Path workingDirectory;

        // Runtime
        private Process? process;

        /// <summary>
        /// Initializes a new instance of the <see cref='RuntimeProcess'/> class.
        /// </summary>
        /// <param name="executable">The executable.</param>
        /// <param name="arguments">The arguemnts.</param>
        /// <param name="workingDirectory">The workingDirectory.</param>
        public RuntimeProcess(
            Path executable,
            string arguments,
            Path workingDirectory)
        {
            this.executable = executable;
            this.arguments = arguments;
            this.workingDirectory = workingDirectory;
        }

        /// <summary>
        /// Execute a process for the provided.
        /// </summary>
        public void Start()
        {
            var processInfo = new ProcessStartInfo()
            {
                FileName = this.executable.ToString(),
                Arguments = this.arguments,
                WorkingDirectory = this.workingDirectory.ToString(),
                RedirectStandardOutput = true,
                RedirectStandardError = true,
            };

            this.process = Process.Start(processInfo);
            if (this.process is null)
                throw new InvalidOperationException("Failed to start process");
        }

        /// <summary>
        /// Wait for the process to exit.
        /// </summary>
        public void WaitForExit()
        {
            if (this.process is null)
                throw new InvalidOperationException("Cannot wait on process that is not running");
            this.process.WaitForExit();
        }

        /// <summary>
        /// Get the exit code.
        /// </summary>
        public int GetExitCode()
        {
            if (this.process is null)
                throw new InvalidOperationException("Cannot access process that does not exist");
            return this.process.ExitCode;
        }

        /// <summary>
        /// Get the standard output.
        /// </summary>
        public string GetStandardOutput()
        {
            if (this.process is null)
                throw new InvalidOperationException("Cannot access process that does not exist");
            return this.process.StandardOutput.ReadToEnd();
        }

        /// <summary>
        /// Get the standard error output.
        /// </summary>
        public string GetStandardError()
        {
            if (this.process is null)
                throw new InvalidOperationException("Cannot access process that does not exist");
            return this.process.StandardError.ReadToEnd();
        }
    }
}
