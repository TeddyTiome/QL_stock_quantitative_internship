#include "include/qlSim.h"
namespace
{
class x2: public AlphaBase
{
    public:
    explicit x2(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        sector(dr.getData<QL_MATRIX<QL_INT>>("sector")),
        Num1(cfg->getAttributeIntDefault("para1",1)),
	Num2(cfg->getAttributeIntDefault("para2",40)),
	Num3(cfg->getAttributeIntDefault("para3",3)),
    Num4(cfg->getAttributeIntDefault("para4",5)),
    Num5(cfg->getAttributeIntDefault("para5",9))
    {

        adv20.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv20[di].resize(GLOBAL::Instruments.size(),nan);
        }
  
        co_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            co_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }
  


    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(), nan);
			vector<float> rank2(GLOBAL::Instruments.size(), nan);
	
       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num2,nan);

                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv20[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                co_ind[di-delay][ii] = close[di-delay][ii] * 0.60733 + open[di-delay][ii] *(1-0.60733); 
            }
        }
            

            QL_Oputils::industryNeutralize(co_ind[di-delay],sector[di-delay]);
            QL_Oputils::industryNeutralize(co_ind[di-delay-Num1],sector[di-delay-Num1]);  
  
      

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

              rank1[ii] = co_ind[di-delay][ii] - co_ind[di-delay-Num1][ii];
            }
        }

        QL_Oputils::rank(rank1);



        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                
                vector<float> c1(Num3,0);
                vector<float> c2(Num5,0);
                vector<float> c3(Num4,0);
                vector<float> c4(Num5,0);

                for (int a=0;a<Num5; ++a){
                    for (int i=0; i<Num3; ++i){
                    c1[i]=vwap[di-delay-i-a][ii];
                    }
                    QL_Oputils::rank(c1);

                    c2[a] = c1[0];
                

                    for (int j=0; j<Num4; ++j){
                    c3[j]=adv20[di-delay-j-a][ii];
                    }
                    QL_Oputils::rank(c3);

                    c4[a] = c3[0];
                }
				
				rank2[ii] = QL_Oputils::corr(c2,c4);

	    }
        }

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
        		if (float(rank1[ii])<float(rank2[ii]))
					{alpha[ii]=1;}
				else
					{alpha[ii]=0;}
			}
		}

        return;
		
    }
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
            ar & adv20;
            ar & co_ind;
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
    const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_FLOAT> &open;
    const QL_MATRIX<QL_INT> &sector;
    vector<vector<float> > adv20;
    vector<vector<float> > co_ind;
        int Num1; 
	int Num2;
	int Num3;
    int Num4;
    int Num5;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new x2(cfg);
        return str;
    }
}

