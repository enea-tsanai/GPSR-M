#include "gpsr-ptable.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision

NS_LOG_COMPONENT_DEFINE ("GpsrTable");


namespace ns3 {
namespace gpsr {

/*
  GPSR position table
*/

PositionTable::PositionTable ()
{
  m_txErrorCallback = MakeCallback (&PositionTable::ProcessTxError, this);
  m_entryLifeTime = Seconds (2); //FIXME fazer isto parametrizavel de acordo com tempo de hello

}

Time 
PositionTable::GetEntryUpdateTime (Ipv4Address id)
{
  if (id == Ipv4Address::GetZero ())
    {
      return Time (Seconds (0));
    }
  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i = m_table.find (id);
  return i->second.second;
}

  
/**
 * \brief Adds entry in position table
 */
void 
PositionTable::AddEntry (Ipv4Address id, Vector position, Vector velocity, double snr)
{
 
  NodeInfo node_info;
  
  /* Keep positioning, velocity and snr in NodeInfo struct*/
  node_info.pos = position;
  node_info.vel = velocity;
  node_info.snr = snr;
  
  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i = m_table.find (id);
  if (i != m_table.end () || id.IsEqual (i->first))
    {
      m_table.erase (id);  
      m_table.insert (std::make_pair (id, std::make_pair (node_info, Simulator::Now ())));
      return;
    }
  m_table.insert (std::make_pair (id, std::make_pair (node_info, Simulator::Now ())));
}

/**
 * \brief Deletes entry in position table
 */
void PositionTable::DeleteEntry (Ipv4Address id)
{
  m_table.erase (id);
}

/**
 * \brief Gets position from position table
 * \param id Ipv4Address to get position from
 * \return Position of that id or PositionTable::GetInvalidPosition () if not known
 */
Vector 
PositionTable::GetPosition (Ipv4Address id)
{
    /*Patched*/
  NodeList::Iterator listEnd = NodeList::End ();
  for (NodeList::Iterator i = NodeList::Begin (); i != listEnd; i++)
    {
      Ptr<Node> node = *i;
      if (node->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal () == id)
        {
          return node->GetObject<MobilityModel> ()->GetPosition ();
        }
    }
  return PositionTable::GetInvalidPosition ();
  
// std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i = m_table.find (id);
//  if (i != m_table.end () || id.IsEqual (i->first))
//    {
//      return i->second.first.pos;
//    }
//
//  return PositionTable::GetInvalidPosition ();
}

/**
 * \brief Checks if a node is a neighbour
 * \param id Ipv4Address of the node to check
 * \return True if the node is neighbour, false otherwise
 */
bool
PositionTable::isNeighbour (Ipv4Address id)
{

 std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i = m_table.find (id);
  if (i != m_table.end () || id.IsEqual (i->first))
    {
      return true;
    }

  return false;
}


/**
 * \brief remove entries with expired lifetime
 */
void 
PositionTable::Purge ()
{
//    std::cout << "----------------Purge--------------" << "\n";
  if(m_table.empty ())
    {
      return;
    }

  std::list<Ipv4Address> toErase;

  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i = m_table.begin ();
  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator listEnd = m_table.end ();
  
  for (; !(i == listEnd); i++)
    {

      if (m_entryLifeTime + GetEntryUpdateTime (i->first) <= Simulator::Now ())
        {
          toErase.insert (toErase.begin (), i->first);

        }
    }
  toErase.unique ();

  std::list<Ipv4Address>::iterator end = toErase.end ();

  for (std::list<Ipv4Address>::iterator it = toErase.begin (); it != end; ++it)
    {

      m_table.erase (*it);

    }
}

/**
 * \brief clears all entries
 */
void 
PositionTable::Clear ()
{
  m_table.clear ();
}



bool inSameRoadandDir (Vector aPos, Vector aVel, Vector bPos, Vector bVel)
{
//    std::cout<<"aVel: " << aVel << " bVel: " << bVel << "\n";
    double road_width = 20.0; // meters
    
    /* If both are moving */
    if (!(aVel.x ==0.0 && aVel.y ==0.0) && !(bVel.x ==0.0 && bVel.y == 0.0))
    {
//        std::cout << "Angle: " << 
//                  acos( (aVel.x*bVel.x+aVel.y*bVel.y)
//                / (sqrt(pow(aVel.x, 2.0)+pow(aVel.y, 2.0)) * sqrt(pow(bVel.x, 2.0)+pow(bVel.y, 2.0))) )
//                * 180 / 3.14159265
//                << " degrees\n";

        /* If angle < 0.05 degrees they are parallel and in same direction*/
        if ((acos( (aVel.x*bVel.x+aVel.y*bVel.y)
                / (sqrt(pow(aVel.x, 2.0)+pow(aVel.y, 2.0)) * sqrt(pow(bVel.x, 2.0)+pow(bVel.y, 2.0))) )
                * 180 / 3.14159265) < 0.1) 
        {
            /* Both moving with x speed>0 */
            if ((aVel.x != 0) && (bVel.x != 0))
            {
                double a_angle = double(aVel.y / aVel.x);
                double a_b = aPos.y - a_angle * aPos.x;

                double b_angle = double(bVel.y / bVel.x);
                double b_b = bPos.y - b_angle * bPos.x;  

                // Same road and direction.. Maybe remove checking again the angle here
                if ((abs(a_angle - b_angle) < 1) && (abs(a_b - b_b) < road_width))
                {
                    return true;                
                }
                return false; 
            }
            /* Movement only at Y*/
            else if (abs(aPos.x - bPos.x) < road_width)
            {
                return true;                
            }
            else
            {
                return false;
            }            
        }
        return false;                 
    }
    // a is moving and b is stationary
    else if(!(aVel.x == 0.0 && aVel.y == 0.0)
            && (bVel.x == 0.0 && bVel.y == 0.0))
    {
        if (aVel.x != 0.0)
        {
            double a_angle = double(aVel.y / aVel.x);
            double a_b = aPos.y - a_angle * aPos.x;
            double b_b = bPos.y - a_angle * bPos.x;
            /* Check if both points are in same line */
            if (abs(a_b - b_b) < road_width)
            {
                return true;
            }
            return false;
        }
        // Movement only on Y axis
        else if ((abs(aPos.x - bPos.x) < road_width) && 
                (CalculateDistance( Vector(aPos.x, aPos.y*(1+aVel.y), 0.0), bPos ) 
                < CalculateDistance(aPos, bPos)))
        {    
            return true;
        }
        else
        {
            return false;
        }        
    }
    // b is moving and a is stationary
    else if((aVel.x == 0.0 && aVel.y == 0.0) 
            && !(bVel.x == 0.0 && bVel.y == 0.0))
    {   
        if (bVel.x != 0.0)
        {
            double b_angle = double(bVel.y / bVel.x);
            double b_b = bPos.y - b_angle * bPos.x;
            double a_b = aPos.y - b_angle * aPos.x;

            if (abs(a_b - b_b) < road_width)
            {
                return true;
            }
            return false;
        }
        // Movement only on Y axis
        else if ((abs(aPos.x - bPos.x) < road_width) && 
                (CalculateDistance( Vector(bPos.x, bPos.y*(1+bVel.y), 0.0), aPos ) 
                < CalculateDistance(aPos, bPos)))
        {
            
                return true;
        }
        else
        {
            return false;
        }        
    }
    return false;
}


double dt(double speed)
{
  if (speed < 5.55) /*20 km/h*/
    {
      return 4.0;
    }
  else if (speed < 11.11) /*40 km/h*/
    {
      return 3.0;
    }
  else if (speed < 16.66) /*60 km/h*/
    {
      return 2.0; 
    }
  else if (speed < 22.22) /*80 km/h*/
    {
      return 2.0; 
    }
  else if (speed < 33.33) /*120 km/h*/
    {
      return 1.5; 
    }
  else if (speed < 55.55) /*200 km/h*/
    {
      return 1.0; 
    }
  return 1.0;
}

/**
 * \brief Calculates the W factor for the next hop
 * \param nodePos the node's current position
 * \dstPos the destination's position
 * \Velocity the node's velocity
 * \return W the Weight of the next hop node 
 */
double calculateW (Vector nodePos, Vector nodeVel, Vector dstPos, Vector dstVel, Vector srcPos, Vector srcVel, double snr, Ipv4Address n)
//double calculateW (Vector nodePos, Vector dstPos, Vector velocity)
{
  double w1 = 0.0, w2 = 0.0, w3 = 0.0, w4 = 0.0, w5 = 0.0;
  
  double srcSpeed = sqrt(pow(srcVel.x, 2.0) + pow(srcVel.y, 2.0));
  double nodeSpeed = sqrt(pow(nodeVel.x, 2.0) + pow(nodeVel.y, 2.0));
  double dstSpeed = sqrt(pow(dstVel.x, 2.0) + pow(dstVel.y, 2.0));
 
  Vector srcFutPos;
  Vector nodeFutPos;
  Vector dstFutPos;
  
  srcFutPos.x = srcPos.x + srcVel.x * dt(srcSpeed);
  srcFutPos.y = srcPos.y + srcVel.y * dt(srcSpeed);
  
  nodeFutPos.x = nodePos.x + nodeVel.x * dt(nodeSpeed);
  nodeFutPos.y = nodePos.y + nodeVel.y * dt(nodeSpeed);
  
  dstFutPos.x = dstPos.x + dstVel.x * dt(dstSpeed);
  dstFutPos.y = dstPos.y + dstVel.y * dt(dstSpeed);
  
//  double Node_SrcCurDist = CalculateDistance (nodePos, srcPos);
  double Node_SrcFutDist = CalculateDistance (nodeFutPos, srcFutPos);
  double Node_DestCurDist = CalculateDistance (nodePos, dstPos);
  double Node_DestFutDist = CalculateDistance (nodeFutPos, dstFutPos);
  
  w1 = 0.25;
  w2 = 0.25;
  w3 = 0.15;
  w4 = 0.15;
  w5 = 1;
  
//  if (nodeVel.x < 4.0 && nodeVel.y < 4.0)
//  {
//    w1 = 1;
//    w2 = 0.0;
//    w3 = 0.0;
//    w4 = 0.0;
//    w5 = 1.0;
//  }
  
  
  if (inSameRoadandDir (nodePos, nodeVel, dstPos, dstVel))
    {
        w3 = 0.0; 
    }
  if (inSameRoadandDir (nodePos, nodeVel, srcPos, srcVel))
    {
        w4 = 0.0; 
    }
    
  double W =  w1 * Node_DestCurDist 
            + w2 * Node_DestFutDist 
            + w3 * Node_DestCurDist 
            + w4 * Node_SrcFutDist
            + w5 * Node_DestFutDist / (snr + 0.1);

//  if ( (n == "10.1.1.0") || (n == "10.1.1.4") || (n == "10.1.1.7") || (n =="10.1.1.35") || (n =="10.1.1.10") || (n =="10.1.1.59") )
//  {
//      
//        std::cout << "-------------\nNodepos: " << std::setprecision(1) << nodePos
//          << "\ndstPos: " << std::setprecision(1) << dstPos
//          << "\nSrcPos: " << std::setprecision(1) << srcPos
//          << "\nnodeVel: " << std::setprecision(1) << nodeVel
//          << "\nsrcVel: " << std::setprecision(1) << srcVel
//          << "\ndstVel: " << std::setprecision(1) <<dstVel      
//          << "\nnodeFutPos: " << std::setprecision(1) << nodeFutPos
//          << "\nNode_DestCurDist: " << std::setprecision(1) << Node_DestCurDist
//          << "\nNode_DestFutDist: " << std::setprecision(1) << Node_DestFutDist
//          << "\n-------------";
//        
//        
//  std::cout << "\nNode: " << n 
//          << "\nw1: " << w1 * Node_DestCurDist 
//          << "\nw2: " << w2 * Node_DestFutDist 
//          <<"\nw3: " << w3 * Node_DestCurDist 
//          <<"\nw4: " << w4 * Node_SrcFutDist
//          <<"\nw5: " << w5 * Node_DestFutDist / (snr + 0.1) << " (futdst: "<< Node_DestFutDist << "snr: " << snr << ")"
//          <<"\nW -> " << W;
//  }
  return W;
}

/**
 * \brief Gets next hop according to GPSR protocol
 * \param DstPos the position of the destination node
 * \param nodePos the position of the node that has the packet
 * \return Ipv4Address of the next hop, Ipv4Address::GetZero () if no nighbour was found in greedy mode
 */
Ipv4Address 
PositionTable::BestNeighbor (Vector dstPos, Vector dstVel, Vector nodePos, Vector nodeVel)
{
  Purge ();
  if (m_table.empty ())
    {
      NS_LOG_DEBUG ("BestNeighbor table is empty; Position: " << dstPos);
      return Ipv4Address::GetZero ();
    }     //if table is empty (no neighbours)

  Ipv4Address bestFoundID = m_table.begin ()->first;
 
  
  /* The node with the smallest weight W is the best Next Hop
   * W = Distance_now + Distance_future
   * Distance_now: The distance form the destination at Time::Now()
   * Distance_future: The distance form the destination after dt = 1sec
   * Distance_future = CalcDist( TargPos, (Vel*dt + Pos_now))
   */
  
  /* Initial calculation is done with source node's velocity */
  double snr = 0.0;
 
  double initialW = calculateW (nodePos, nodeVel, dstPos, dstVel, nodePos, nodeVel, snr, m_table.begin ()->first);
  
  double W = calculateW (m_table.begin ()->second.first.pos, m_table.begin ()->second.first.vel, 
          dstPos, dstVel, nodePos, nodeVel, m_table.begin ()->second.first.snr, m_table.begin ()->first);
  
  
//        std::cout << "T: " << std::fixed << std::setprecision(4) << Simulator::Now ().GetSeconds()              
//              << " \tNode_pos " << nodePos
//              << " \tDst pos " << dstPos
//              << "\n";

  
//  double W = initialW;
  
//  std::cout << "dstPos: " << dstPos << "\n";
//  std::cout << "Initial W: " << initialW << "\n velocity: " << m_table.begin ()->second.first.vel << "\n";
  
  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i;
  for (i = m_table.begin (); !(i == m_table.end ()); i++)
    {
      std::cout << std::fixed;
      
//      std::cout << "T: " << std::fixed << std::setprecision(4) << Simulator::Now ().GetSeconds() 
//              << " \tNode: " << i->first 
//              << " \tPos: " << std::setprecision(1) << i->second.first.pos
//              << " \tVelocity: " << std::setprecision(1) << i->second.first.vel
//              << " \tSnr: " << i->second.first.snr
//              << " \tDst: " << std::setprecision(1) << dstPos
//              << " \tW: " << std::setprecision(2) << calculateW (i->second.first.pos,
//            i->second.first.vel, dstPos, dstVel, nodePos, nodeVel, i->second.first.snr, i->first) << "\n";
      
      if (W > calculateW (i->second.first.pos,
            i->second.first.vel, dstPos, dstVel, nodePos, nodeVel, i->second.first.snr, i->first))
        {
          bestFoundID = i->first;
          W = calculateW (i->second.first.pos,
            i->second.first.vel, dstPos, dstVel, nodePos, nodeVel, i->second.first.snr, i->first);
        }
    }
//  std::cout << "End of loop --> W: " << W <<"\n";
  if(initialW > W)
  {
//      std::cout << "W: " << W << "\n";
//    std::cout << "initialW > W| bestFoundID:  " << bestFoundID << "\n";
    return bestFoundID;
  }
  else
  {
//    std::cout << "Else| Ipv4Address::GetZero:  " << Ipv4Address::GetZero () << "\n";
    return Ipv4Address::GetZero (); //so it enters Recovery-mode
  }
}


/**
 * \brief Gets next hop according to GPSR recovery-mode protocol (right hand rule)
 * \param previousHop the position of the node that sent the packet to this node
 * \param nodePos the position of the destination node
 * \return Ipv4Address of the next hop, Ipv4Address::GetZero () if no nighbour was found in greedy mode
 */
Ipv4Address
PositionTable::BestAngle (Vector previousHop, Vector nodePos)
{
  Purge ();

  if (m_table.empty ())
    {
      NS_LOG_DEBUG ("BestNeighbor table is empty; Position: " << nodePos);
      return Ipv4Address::GetZero ();
    }     //if table is empty (no neighbours)

  double tmpAngle;
  Ipv4Address bestFoundID = Ipv4Address::GetZero ();
  double bestFoundAngle = 360;
  std::map<Ipv4Address, std::pair<NodeInfo, Time> >::iterator i;

  for (i = m_table.begin (); !(i == m_table.end ()); i++)
    {
      tmpAngle = GetAngle(nodePos, previousHop, i->second.first.pos);
      if (bestFoundAngle > tmpAngle && tmpAngle != 0)
  {
    bestFoundID = i->first;
    bestFoundAngle = tmpAngle;
  }
    }
  if(bestFoundID == Ipv4Address::GetZero ()) //only if the only neighbour is who sent the packet
    {
      bestFoundID = m_table.begin ()->first;
    }
  return bestFoundID;
}


//Gives angle between the vector CentrePos-Refpos to the vector CentrePos-node counterclockwise
double 
PositionTable::GetAngle (Vector centrePos, Vector refPos, Vector node){
  double const PI = 4*atan(1);

  std::complex<double> A = std::complex<double>(centrePos.x,centrePos.y);
  std::complex<double> B = std::complex<double>(node.x,node.y);
  std::complex<double> C = std::complex<double>(refPos.x,refPos.y);   //Change B with C if you want angles clockwise

  std::complex<double> AB; //reference edge
  std::complex<double> AC;
  std::complex<double> tmp;
  std::complex<double> tmpCplx;

  std::complex<double> Angle;

  AB = B - A;
  AB = (real(AB)/norm(AB)) + (std::complex<double>(0.0,1.0)*(imag(AB)/norm(AB)));

  AC = C - A;
  AC = (real(AC)/norm(AC)) + (std::complex<double>(0.0,1.0)*(imag(AC)/norm(AC)));

  tmp = log(AC/AB);
  tmpCplx = std::complex<double>(0.0,-1.0);
  Angle = tmp*tmpCplx;
  Angle *= (180/PI);
  if (real(Angle)<0)
    Angle = 360+real(Angle);

  return real(Angle);

}





/**
 * \ProcessTxError
 */
void PositionTable::ProcessTxError (WifiMacHeader const & hdr)
{
}



//FIXME ainda preciso disto agr que o LS ja n está aqui???????

/**
 * \brief Returns true if is in search for destionation
 */
bool PositionTable::IsInSearch (Ipv4Address id)
{
  return false;
}

bool PositionTable::HasPosition (Ipv4Address id)
{
  return true;
}

void
PositionTable::PrintPositionTable (Ptr<OutputStreamWrapper> stream) const
{
    std::ostream* os = stream->GetStream ();
    *os << "\n Time: "<< Simulator::Now ().GetSeconds() << "\n-----------------\n";  

    /*Print Neighbors*/

    std::map<Ipv4Address, std::pair<NodeInfo, Time> >::const_iterator i, j;
    for (i = m_table.begin (); !(i == m_table.end ()); i++)
      {
        *os << "IP: " << i->first << "\t"
            << "Pos: " << i->second.first.pos.x << ", " << i->second.first.pos.y << "\t"
            << "Vel: " << i->second.first.vel.x << ", " << i->second.first.vel.y << "\t"
            << "Time: " <<  i->second.second.GetSeconds()
            << "\n\n";    
      }
}

}   // gpsr
} // ns3
