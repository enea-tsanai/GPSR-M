/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Pavel Boyko <boyko@iitp.ru>
 */
#include "ns3/test.h"
#include "ns3/gpsr-packet.h"
#include "ns3/gpsr-rqueue.h"
#include "ns3/gpsr-ptable.h"
#include "ns3/ipv4-route.h"

namespace ns3
{
namespace gpsr
{

/// Unit test for neighbors
struct NeighborTest : public TestCase
{
  NeighborTest () : TestCase ("Neighbor") { }
  virtual void DoRun ();
};

void
NeighborTest::DoRun ()
{
  PositionTable nb  = PositionTable ();


  nb.AddEntry (Ipv4Address ("1.2.3.4"), Vector (10, 20, 0));
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("1.2.3.4")), true, "Neighbor exists");
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("4.3.2.1")), false, "Neighbor doesn't exist");

  //test update neighbour
  NS_TEST_EXPECT_MSG_EQ (nb.GetPosition (Ipv4Address ("1.2.3.4")).x, 10, "Correct X position in table");
  NS_TEST_EXPECT_MSG_EQ (nb.GetPosition (Ipv4Address ("1.2.3.4")).y, 20, "Correct Y position in table");
  nb.AddEntry (Ipv4Address ("1.2.3.4"), Vector (30, 40, 0));
  NS_TEST_EXPECT_MSG_EQ (nb.GetPosition (Ipv4Address ("1.2.3.4")).x, 30, "X Position correctly updated");
  NS_TEST_EXPECT_MSG_EQ (nb.GetPosition (Ipv4Address ("1.2.3.4")).y, 40, "Y Position correctly updated");

  nb.AddEntry (Ipv4Address ("4.3.2.1"), Vector (10, 10, 0));
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("1.2.3.4")), true, "Neighbor exists");
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("4.3.2.1")), true, "Neighbor exists");

  nb.DeleteEntry (Ipv4Address ("1.2.3.4"));
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("1.2.3.4")), false, "Neighbor doesn't exist");
  NS_TEST_EXPECT_MSG_EQ (nb.isNeighbour (Ipv4Address ("4.3.2.1")), true, "Neighbor exists");


  //test to select correct neighbour
  nb.AddEntry (Ipv4Address ("1.2.3.4"), Vector (10, 20, 0));
  nb.AddEntry (Ipv4Address ("1.2.3.10"), Vector (30, 30, 0));
  NS_TEST_EXPECT_MSG_EQ (nb.BestNeighbor (Vector (20, 10, 0), Vector (0, 0, 0)), Ipv4Address ("4.3.2.1"), "Found correct neighbour in greedy");
  NS_TEST_EXPECT_MSG_EQ (nb.BestNeighbor (Vector (40, 30, 0), Vector (0, 0, 0)), Ipv4Address ("1.2.3.10"), "Found correct neighbour in greedy");

  //test not to select any neighbour further away from destination
  NS_TEST_EXPECT_MSG_EQ (nb.BestNeighbor (Vector (50, 20, 0), Vector (40, 20, 0)), Ipv4Address::GetZero (), "No neighbour further away to destination selected");
  

  //test selection of correct neighbour in recovery mode
  NS_TEST_EXPECT_MSG_EQ (nb.BestAngle (Vector (30, 30, 0), Vector (20, 30, 0)), Ipv4Address ("4.3.2.1"), "Found correct neighbour in recovery");
  NS_TEST_EXPECT_MSG_EQ (nb.BestAngle (Vector (10, 20, 0), Vector (30, 30, 0)), Ipv4Address ("1.2.3.10"), "Found correct neighbour in recovery");

}
//-----------------------------------------------------------------------------
struct TypeHeaderTest : public TestCase
{
  TypeHeaderTest () : TestCase ("GPSR TypeHeader") 
  {
  }
  virtual void DoRun ()
  {
    TypeHeader h (GPSRTYPE_HELLO);
    NS_TEST_EXPECT_MSG_EQ (h.IsValid (), true, "Default header is valid");
    NS_TEST_EXPECT_MSG_EQ (h.Get (), GPSRTYPE_HELLO, "Default header is RREQ");

    Ptr<Packet> p = Create<Packet> ();
    p->AddHeader (h);
    TypeHeader h2 (GPSRTYPE_POS);
    uint32_t bytes = p->RemoveHeader (h2);
    NS_TEST_EXPECT_MSG_EQ (bytes, 1, "Type header is 1 byte long");
    NS_TEST_EXPECT_MSG_EQ (h, h2, "Round trip serialization works");
  }
};
//-----------------------------------------------------------------------------
/// Unit test for HELLO Packet
struct HelloHeaderTest : public TestCase
{
  HelloHeaderTest () : TestCase ("GPSR HELLO") 
  {
  }
  virtual void DoRun ()
  {
    HelloHeader h (/*PositionX*/ 20, /*PositionY*/ 10);
    NS_TEST_EXPECT_MSG_EQ (h.GetOriginPosx (), 20, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetOriginPosy (), 10, "trivial");

    h.SetOriginPosx (2);
    NS_TEST_EXPECT_MSG_EQ (h.GetOriginPosx (), 2, "trivial");
    h.SetOriginPosy (30);
    NS_TEST_EXPECT_MSG_EQ (h.GetOriginPosy (), 30, "trivial");

    Ptr<Packet> p = Create<Packet> ();
    p->AddHeader (h);
    HelloHeader h2;
    uint32_t bytes = p->RemoveHeader (h2);
    NS_TEST_EXPECT_MSG_EQ (bytes, 16, "HelloHeader is 16 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h, h2, "Round trip serialization works");

  }
};
//-----------------------------------------------------------------------------
/// Unit test for Position Packet
struct PositionHeaderTest : public TestCase
{
  PositionHeaderTest () : TestCase ("GPSR POS") {}
  virtual void DoRun ()
  {
    PositionHeader h (/*dstPosx*/ 1, /*dstPosy*/ 2, /*updated*/ 10, /*recPosx*/ 0, /*recPosy*/ 0, /*inRec*/ 0, /*lastPosx*/ 20, /*lastPosy*/ 15);
    NS_TEST_EXPECT_MSG_EQ (h.GetDstPosx (), 1, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetDstPosy (), 2, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetUpdated (), 10, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetRecPosx (), 0, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetRecPosy (), 0, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetInRec (), 0, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetLastPosx (), 20, "trivial");
    NS_TEST_EXPECT_MSG_EQ (h.GetLastPosy (), 15, "trivial");

    h.SetDstPosx (3);
    NS_TEST_EXPECT_MSG_EQ (h.GetDstPosx (), 3, "trivial");
    h.SetDstPosy (4);
    NS_TEST_EXPECT_MSG_EQ (h.GetDstPosy (), 4, "trivial");
    h.SetUpdated (11);
    NS_TEST_EXPECT_MSG_EQ (h.GetUpdated (), 11, "trivial");
    h.SetRecPosx (6);
    NS_TEST_EXPECT_MSG_EQ (h.GetRecPosx (), 6, "trivial");
    h.SetRecPosy (2);
    NS_TEST_EXPECT_MSG_EQ (h.GetRecPosy (), 2, "trivial");
    h.SetInRec (1);
    NS_TEST_EXPECT_MSG_EQ (h.GetInRec (), 1, "trivial");
    h.SetLastPosx (23);
    NS_TEST_EXPECT_MSG_EQ (h.GetLastPosx (), 23, "trivial");
    h.SetLastPosy (17);
    NS_TEST_EXPECT_MSG_EQ (h.GetLastPosy (), 17, "trivial");


    Ptr<Packet> p = Create<Packet> ();
    p->AddHeader (h);
    PositionHeader h2;
    uint32_t bytes = p->RemoveHeader (h2);
    NS_TEST_EXPECT_MSG_EQ (bytes, 53, "POS is 53 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h, h2, "Round trip serialization works");
  }
};
//-----------------------------------------------------------------------------
/// Unit test for RequestQueue
struct GpsrRqueueTest : public TestCase
{
  GpsrRqueueTest () : TestCase ("Rqueue"), q (64, Seconds (30)) {}
  virtual void DoRun ();
  void Unicast (Ptr<Ipv4Route> route, Ptr<const Packet> packet, const Ipv4Header & header) {}
  void Error (Ptr<const Packet>, const Ipv4Header &, Socket::SocketErrno) {}
  void CheckSizeLimit ();
  void CheckTimeout ();

  RequestQueue q;
};

void
GpsrRqueueTest::DoRun ()
{
  NS_TEST_EXPECT_MSG_EQ (q.GetMaxQueueLen (), 64, "trivial");
  q.SetMaxQueueLen (32);
  NS_TEST_EXPECT_MSG_EQ (q.GetMaxQueueLen (), 32, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.GetQueueTimeout (), Seconds (30), "trivial");
  q.SetQueueTimeout (Seconds (10));
  NS_TEST_EXPECT_MSG_EQ (q.GetQueueTimeout (), Seconds (10), "trivial");

  Ptr<const Packet> packet = Create<Packet> ();
  Ipv4Header h;
  h.SetDestination (Ipv4Address ("1.2.3.4"));
  h.SetSource (Ipv4Address ("4.3.2.1"));
  Ipv4RoutingProtocol::UnicastForwardCallback ucb = MakeCallback (&GpsrRqueueTest::Unicast, this);
  Ipv4RoutingProtocol::ErrorCallback ecb = MakeCallback (&GpsrRqueueTest::Error, this);
  QueueEntry e1 (packet, h, ucb, ecb, Seconds (1));
  q.Enqueue (e1);
  q.Enqueue (e1);
  q.Enqueue (e1);
  NS_TEST_EXPECT_MSG_EQ (q.Find (Ipv4Address ("1.2.3.4")), true, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.Find (Ipv4Address ("1.1.1.1")), false, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 1, "trivial");
  q.DropPacketWithDst (Ipv4Address ("1.2.3.4"));
  NS_TEST_EXPECT_MSG_EQ (q.Find (Ipv4Address ("1.2.3.4")), false, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 0, "trivial");

  h.SetDestination (Ipv4Address ("2.2.2.2"));
  QueueEntry e2 (packet, h, ucb, ecb, Seconds (1));
  q.Enqueue (e1);
  q.Enqueue (e2);
  Ptr<Packet> packet2 = Create<Packet> ();
  QueueEntry e3 (packet2, h, ucb, ecb, Seconds (1));
  NS_TEST_EXPECT_MSG_EQ (q.Dequeue (Ipv4Address ("3.3.3.3"), e3), false, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.Dequeue (Ipv4Address ("2.2.2.2"), e3), true, "trivial");
  NS_TEST_EXPECT_MSG_EQ (q.Find (Ipv4Address ("2.2.2.2")), false, "trivial");
  q.Enqueue (e2);
  q.Enqueue (e3);
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 2, "trivial");
  Ptr<Packet> packet4 = Create<Packet> ();
  h.SetDestination (Ipv4Address ("1.2.3.4"));
  QueueEntry e4 (packet4, h, ucb, ecb, Seconds (20));
  q.Enqueue (e4);
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 3, "trivial");
  q.DropPacketWithDst (Ipv4Address ("1.2.3.4"));
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 1, "trivial");

  CheckSizeLimit ();

  Ipv4Header header2;
  Ipv4Address dst2 ("1.2.3.4");
  header2.SetDestination (dst2);

  Simulator::Schedule (q.GetQueueTimeout () + Seconds (1), &GpsrRqueueTest::CheckTimeout, this);

  Simulator::Run ();
  Simulator::Destroy ();
}

void
GpsrRqueueTest::CheckSizeLimit ()
{
  Ptr<Packet> packet = Create<Packet> ();
  Ipv4Header header;
  Ipv4RoutingProtocol::UnicastForwardCallback ucb = MakeCallback (&GpsrRqueueTest::Unicast, this);
  Ipv4RoutingProtocol::ErrorCallback ecb = MakeCallback (&GpsrRqueueTest::Error, this);
  QueueEntry e1 (packet, header, ucb, ecb, Seconds (1));

  for (uint32_t i = 0; i < q.GetMaxQueueLen (); ++i)
    q.Enqueue (e1);
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 2, "trivial");

  for (uint32_t i = 0; i < q.GetMaxQueueLen (); ++i)
    q.Enqueue (e1);
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 2, "trivial");
}

void
GpsrRqueueTest::CheckTimeout ()
{
  NS_TEST_EXPECT_MSG_EQ (q.GetSize (), 0, "Must be empty now");
}
//-----------------------------------------------------------------------------
class GpsrTestSuite : public TestSuite
{
public:
  GpsrTestSuite () : TestSuite ("routing-gpsr", UNIT)
  {
    AddTestCase (new NeighborTest);
    AddTestCase (new TypeHeaderTest);
    AddTestCase (new HelloHeaderTest);
    AddTestCase (new PositionHeaderTest);
    AddTestCase (new GpsrRqueueTest);
  }
} g_gpsrTestSuite;

}
}
