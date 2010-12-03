package cast.examples;

import cast.CASTException;
import cast.cdl.ComponentLanguage;
import cast.core.CASTUtils;
import cast.examples.autogen.WordServer;
import cast.examples.autogen.WordServerPrx;

public class ExternalClientTest {

	public static void main(String[] args) throws CASTException,
			InterruptedException {
		WordServerPrx wordServer = CASTUtils.getCASTIceServer(
				"implements.server", "localhost", ComponentLanguage.JAVA,
				WordServer.class, WordServerPrx.class);

		while (true) {
			System.out.println(wordServer.getNewWord());
			Thread.sleep(1000);
		}

	}

}
