namespace PelTaskSharp.Operations
{
    public class DivideOperation : IOperation
    {
        public double Call(params double[] args)
        {
            if (args.Length != 2)
            {
                throw new ArgumentException("Division requires exactly two arguments.");
            }

            double dividend = args[0];
            double divisor = args[1];

            if (divisor == 0)
            {
                throw new ArgumentException("Divisor cannot be zero.");
            }

            return dividend / divisor;
        }
    }
}