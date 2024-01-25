using PelTaskSharp.Operations;

namespace PelTaskSharp
{
    public class Calculator
    {
        private Dictionary<string, IOperation> operations;

        public Calculator()
        {
            operations = new Dictionary<string, IOperation>(StringComparer.OrdinalIgnoreCase)
            {
                { "+", new AddOperation() },
                { "-", new SubtractOperation() },
                { "*", new MultiplyOperation() },
                { "/", new DivideOperation() },
                { "sin", new SinOperation() },
                { "atan2", new Atan2Operation() }
            };
        }

        public void Run()
        {
            Console.WriteLine("Welcome to the Calculator!");

            while (true)
            {
                Console.WriteLine("Enter an operation:");
                string operation = Console.ReadLine();

                Console.WriteLine("Enter arguments:");
                string argsString = Console.ReadLine();

                try
                {
                    if (string.IsNullOrWhiteSpace(argsString))
                    {
                        throw new ArgumentException("At least one argument is required.");
                    }
                    string[] args = argsString.Split(new char[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                    double[] parsedArgs = Array.ConvertAll(args, double.Parse);

                    if (operations.TryGetValue(operation.Trim(), out IOperation selectedOperation))
                    {
                        double result = selectedOperation.Call(parsedArgs);
                        Console.WriteLine($"Result: {result}");
                    }
                    else
                    {
                        Console.WriteLine("Invalid operation.");
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error: {ex.Message}");
                }

                Console.WriteLine();
            }
        }
    }
}