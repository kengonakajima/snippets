import { lookupName } from "namemc";

// Async function to wrap await methods
(async function(){

	// Look up users that have the name or have had the name '3E7'
	const users = await lookupName("3E7");

	// Log each user to console
	users.map(user => {
		console.log(user);
	});

}())
