﻿namespace PelTaskSharp.Operations
{
    public class MultiplyOperation : IOperation
    {
        public double Call(params double[] args)
        {
            if (args.Length < 2)
            {
                throw new ArgumentException("Multiplication requires at least two arguments.");
            }

            double result = args[0];
            for (int i = 1; i < args.Length; i++)
            {
                result *= args[i];
            }

            return result;
        }
    }
}