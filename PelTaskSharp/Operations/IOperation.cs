namespace PelTaskSharp.Operations
{
    internal interface IOperation
    {
        double Call(params double[] args);
    }
}