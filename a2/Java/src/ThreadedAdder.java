import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Scanner;

public class ThreadedAdder {
    public static int[] units;
    public static int[] carries;
    public static void main(String [] args  ) throws InterruptedException, IOException {
//        int[][] arr2 = {
//                {6, 9, 4, 3},
//                {9, 4, 3, 9},
//                {8, 8, 8, 8},
//                {9, 9, 9, 9}
//        };
//
//        int[][] arr = {
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9},
//                {9, 9, 9}
//        };
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
//        String  lines = br.readLine();
        System.out.println("Enter each number in this format: 9 9 9");
        System.out.println("Enter each number on a new line, hit enter a second time to add");
        System.out.println("Note: all numbers need to have the same number of digits. Enter leading zeros for shorter numbers.");

//        int[][] arr;

        ArrayList<Integer[]> arrList = new ArrayList<Integer[]>();
        while(true) {
            System.out.println("Enter each number to be added, with each digit separated by a space, and each number separated by a line.");
            while (true) {
                String line = br.readLine();
                if (line.equals("")) {
                    break;
                }
                String[] strs = line.trim().split("\\s+");
                ArrayList<Integer> number = new ArrayList<Integer>();
                for (String str : strs) {
                    number.add(Integer.parseInt(str));
                }
                arrList.add(number.toArray(new Integer[0]));
            }
            Integer[][] arr = arrList.toArray(new Integer[0][0]);
            adder(arr);
        }
    }

    public static void adder(Integer[][] arr) throws InterruptedException {
        int totalNums = arr.length;
        int numDigits = arr[0].length;
        units = new int[numDigits];
        carries = new int[numDigits + 1];

        Thread[] adders = new Thread[numDigits];

        for(int i = 0; i < numDigits; i++)
        {
            adders[i] = new Thread(new AddThread(i, numDigits, totalNums, arr));
            adders[i].start();
        }

        for (int i = 0; i < numDigits; i++)
        {
            adders[i].join();
        }
        
        int[] result = new int[numDigits + 2];
        int[] secondCarry = new int[numDigits + 2];
        for(int i = numDigits + 1; i > 1; i--)
        {
            int tot = units[i-2] + carries[i-1];
            result[i] = tot % 10;
            secondCarry[i-1] = tot / 10;
        }
        result[1] = carries[0] % 10;
        result[0] = carries[0] / 10;

        for(int i = numDigits + 1; i >= 0; i--)
        {
            result[i] += secondCarry[i];
        }

        boolean hasPrinted = false;
        System.out.print("result: ");
        for (int i = 0; i < numDigits + 2; i++)
        {
            if (result[i] != 0 || hasPrinted)
            {
                System.out.printf("%d ", result[i]);
                hasPrinted = true;
            }
        }
        System.out.print("\n");



    }

    static class AddThread implements Runnable {
        int column, numDigits, totalNums;
        Integer[][] arr;
        int total = 0;

        public AddThread(int col, int n, int t, Integer[][] a){
            column = col;
            numDigits = n;
            totalNums = t;
            arr = a;
        }
        public void run() {
            for(int i = 0; i < totalNums; i++)
            {
                total += arr[i][column];
            }
            int carry = total / 10;
            int remainder = total % 10;
            carries[column] = carry;
            units[column] = remainder;

        }
    }
}

