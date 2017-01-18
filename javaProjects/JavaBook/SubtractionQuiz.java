import java.util.Scanner;

public class SubtractionQuiz {
	public static void main(String[] args) {

		int num1 = (int)(Math.random() * 10);
		int num2 = (int)(Math.random() * 10);

		if (num1 < num2) {
			int tmp = num1;
			num1 = num2;
			num2 = tmp;
		}

		Scanner input = new Scanner(System.in);
		System.out.print("What is " + num1 + " - " + num2 + "? ");
		int answer = input.nextInt();

		System.out.println(num1 + " - " + num2 + " = " + answer + " is " + (num1 - num2 == answer));
	}
}