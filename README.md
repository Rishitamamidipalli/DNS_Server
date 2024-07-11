# DNS Network Server (Recursive Model):
### Introduction:
This project simulates a DNS (Domain Name System) network using a recursive model. The system includes a local DNS server, a root server, a TLD (Top-Level Domain) server, and an authoritative server. When a client sends a URL, the servers work together to convert the URL and domain name into an IP address that computers can understand to locate the corresponding webpage.

### Difference between Recursive DNS and Iterative DNS
A recursive DNS lookup is where one DNS server communicates with several other DNS servers to hunt down an IP address and return it to the client. This is in contrast to an iterative DNS query, where the client communicates directly with each DNS server involved in the lookup. While this is a very technical definition, a closer look at the DNS system and the difference between recursion and iteration should help clear things up.
![Screenshot 2024-07-11 131649](https://github.com/Rishitamamidipalli/DNS_Server/assets/123208162/72f56f90-900e-46c3-b37e-d48279557b98)

### Working of Recursive DNS

1. **Checks if the IP address is stored in the cache memory**:There is a certain period of time, pre-defined by the domain’s owner called Time to Live or TTL. It says for how long the Recursive server can hold the information. If it is still there, it will return the answer fast and won’t take further actions.
2. **Searches for the IP address elsewhere**: If it is not in the cache, it will continue the searching process until it gets to an Authoritative server which has the information.
  - The DNS resolver is the one that obtains the DNS query from the user.
  - It then asks the Root server about the location of the TLD (Top Level Domain) server.
  - The Recursive queries the TLD (Top Level Domain) server for information about which is the accountable Authoritative DNS server for the precise domain.
  - It makes a request to the Authoritative DNS server responsible for the particular domain. 
  - The Resolver gets back to the user and provides the requested data.
  - It caches the DNS information for further use.
