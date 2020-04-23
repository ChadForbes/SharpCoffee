using System;

namespace InsertionSort
{
    class InsertionSort
    {
        public static void Main()
        {
            /* Sorted array
             * { 1, 2, 2, 2, 3, 3, 4, 5, 6, 7 }
            */
            int[] arr = { 2, 6, 3, 2, 4, 3, 1, 7, 2, 5 };
            printArray(arr);
            InsertionSort iSort = new InsertionSort();
            iSort.sort(arr);
            printArray(arr);
        }

        void sort(int[] arr)
        {
            int n = arr.Length;
            for (int i = 1; i < n; i++)
            {
                int key = arr[i];
                int j = i - 1;

                while (j >= 0 && arr[j] > key)
                {
                    arr[j + 1] = arr[j];
                    j = j - 1;
                }
                arr[j + 1] = key;
            }
        }

        static void printArray(int[] arr)
        {
            int n = arr.Length;
            for (int i = 0; i < n; ++i)
                Console.Write(arr[i] + " ");

            Console.Write("\n");
        }
    }
}
