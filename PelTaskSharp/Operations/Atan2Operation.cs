namespace PelTaskSharp.Operations
{
    public class Atan2Operation : IOperation
    {
        public double Call(params double[] args)
        {
            if (args.Length != 2)
            {
                throw new ArgumentException("Atan2 function requires exactly two arguments.");
            }

            return Math.Atan2(args[0], args[1]);
        }
    }
}