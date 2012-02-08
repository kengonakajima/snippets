import pat.*;


class aho {
	public static void main(String[] notused) {
		Regex r = new Regex("[a-c]+([x-z]+)");
		r.search("abdcxyz");
		System.out.println("match => "+r.substring());
//		System.out.println("backrefernce 0 => "+r.substring(0));
	}
}
