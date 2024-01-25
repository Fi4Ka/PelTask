namespace PelTaskSharp.Operations
{
    public class SinOperation : IOperation
    {
        public double Call(params double[] args)
        {
            if (args.Length != 1)
            {
                throw new ArgumentException("Sin function requires exactly one argument.");
            }

            return Math.Sin(args[0]);
        }
    }
}