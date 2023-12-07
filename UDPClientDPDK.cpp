#include <iostream>
#include <cstring>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_udp.h>

#define RTE_MAX_ETHPORTS 32
#define RTE_MAX_LCORES 64

static const char* server_ip = "10.0.0.23";
static const uint16_t server_port = 8000;
static const uint16_t udp_port = 1234; // Adjust this to your needs

void init_dpdk() {
    int argc = 1;
    char* argv[] = {strdup("your_program_name"), nullptr};
    rte_eal_init(argc, argv);
}

void start_client(uint16_t port_id) {
    // Initialize the UDP socket
    int client_socket = rte_eth_dev_socket_id(port_id);

    // Server address information
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_address.sin_addr);

    // Create a packet buffer (m_buf) for sending UDP packets
    struct rte_mbuf* m_buf = rte_pktmbuf_alloc(rte_pktmbuf_pool_default());
    if (!m_buf) {
        std::cerr << "Error allocating mbuf\n";
        return;
    }

    // Set up the UDP packet
    struct rte_udp_hdr* udp_hdr = rte_pktmbuf_mtod_offset(m_buf, struct rte_udp_hdr*, 0);
    udp_hdr->src_port = rte_cpu_to_be_16(udp_port);
    udp_hdr->dst_port = rte_cpu_to_be_16(server_port);
    m_buf->pkt_len = m_buf->data_len = sizeof(struct rte_udp_hdr);

    // Sending UDP packets in a loop
    while (1) {
        // Send the packet to the server
        ssize_t sent_bytes = sendto(client_socket, rte_pktmbuf_mtod(m_buf, void*), m_buf->pkt_len, 0,
                                    (struct sockaddr*)&server_address, sizeof(server_address));
        if (sent_bytes < 0) {
            std::cerr << "Error sending UDP packet\n";
            break;
        }

        std::cout << "Packet sent\n";
    }

    // Release the mbuf when done
    rte_pktmbuf_free(m_buf);
}

int main() {
    init_dpdk();

    uint16_t port_id = 0; // Adjust the port ID based on your system configuration

    if (rte_eth_dev_is_valid_port(port_id) == 0) {
        std::cerr << "Invalid port ID\n";
        return -1;
    }

    createAndStartThread(1, "client", start_client, port_id);

    rte_eal_mp_wait_lcore();

    return 0;
}
