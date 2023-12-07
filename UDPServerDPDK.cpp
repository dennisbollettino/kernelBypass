#include <iostream>
#include <thread_utils.h>
#include <rte_common.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_udp.h>

#define PORT 8000
#define RX_RING_SIZE 1024
#define MBUF_POOL_SIZE 8191
#define MBUF_CACHE_SIZE 250

static const uint16_t udp_port = 1234; // Adjust this to your needs

int start_server(uint16_t port_id) {
    // Initialize the DPDK environment
    int argc = 1;
    char* argv[] = {strdup("your_program_name"), nullptr};
    rte_eal_init(argc, argv);

    // Create a UDP socket using DPDK
    int serv_socket = rte_eth_dev_socket_id(port_id);

    // Initialize the DPDK memory pool for packet buffers
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("mbuf_pool",
            MBUF_POOL_SIZE, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

    if (!mbuf_pool) {
        std::cerr << "Error creating mbuf pool\n";
        return -1;
    }

    // Setup receive queue configuration
    struct rte_eth_conf port_conf = {0};
    struct rte_eth_rxconf rx_conf = {0};
    rx_conf.rx_drop_en = 1;

    // Initialize the Ethernet device
    if (rte_eth_dev_configure(port_id, 1, 1, &port_conf) < 0
        || rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &RX_RING_SIZE, NULL, NULL) < 0
        || rte_eth_rx_queue_setup(port_id, 0, RX_RING_SIZE, rte_eth_dev_socket_id(port_id), &rx_conf, mbuf_pool) < 0) {
        std::cerr << "Error configuring Ethernet device\n";
        return -1;
    }

    // Start the Ethernet device
    if (rte_eth_dev_start(port_id) < 0) {
        std::cerr << "Error starting Ethernet device\n";
        return -1;
    }

    // Receive and process UDP packets in a loop
    struct rte_mbuf *m_bufs[RX_RING_SIZE];
    while (1) {
        const uint16_t nb_rx = rte_eth_rx_burst(port_id, 0, m_bufs, RX_RING_SIZE);
        for (uint16_t i = 0; i < nb_rx; i++) {
            std::cout << "Packet received\n";
            char* packet_data = rte_pktmbuf_mtod(m_bufs[i], char*);
            std::cout << "The message was: " << packet_data << std::endl;
            rte_pktmbuf_free(m_bufs[i]); // Free the received packet buffer
        }
    }

    return 0;
}

int main() {
    // Adjust the port ID based on your system configuration
    uint16_t port_id = 0;

    if (rte_eth_dev_is_valid_port(port_id) == 0) {
        std::cerr << "Invalid port ID\n";
        return -1;
    }

    auto t1 = createAndStartThread(1, "udp-server", start_server, port_id);
    t1->join();

    rte_eal_cleanup(); // Cleanup DPDK environment

    return 0;
}
