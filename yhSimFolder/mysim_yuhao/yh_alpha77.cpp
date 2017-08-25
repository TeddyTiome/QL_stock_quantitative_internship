#include "include/qlSim.h"
namespace
{
class alpha77: public AlphaBase
{
    public:
    explicit alpha77(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",40)),
	Num3(cfg->getAttributeIntDefault("para3",3)),
	Num4(cfg->getAttributeIntDefault("para4",5))
    {

        adv40.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv40[di].resize(GLOBAL::Instruments.size(),nan);
        }

        hlv.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            hlv[di].resize(GLOBAL::Instruments.size(),nan);
        }

        hl.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            hl[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv40[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                hl[di-delay][ii] = 0.5*(high[di-delay][ii]+low[di-delay][ii]);
                hlv[di-delay][ii] = 0.5*(high[di-delay][ii]+low[di-delay][ii])-vwap[di-delay][ii];
            }
        }


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num1,nan);
                rank1[ii] = 0.0;

                for (int i=0; i< Num1;++i){
                c1[i]= hlv[di-delay-i][ii];
                rank1[ii] += float(c1[i])*(Num1-i) / ((Num1+1)*Num1/2.0);
                }
            }
        }

        QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                rank2[ii] = 0.0;            	
               	vector<float> c2(Num3, nan);               	
               	vector<float> c3(Num3, nan);               	
               	vector<float> c4(Num4, nan);    

			for (int i=0; i<Num4;++i){
			for (int j=0; j <Num3;++j){
				c2[j]= hl[di-delay-j-i][ii];
			     c3[j]=adv40[di-delay-j-i][ii];
			}
			c4[i] = QL_Oputils::corr(c2,c3);
			rank2[ii] += float(c4[i])*(Num4-i) / ((Num4+1)*Num4/2.0);
			}
	    }
        }

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                if (rank1[ii]<rank2[ii])
                    {alpha[ii] = float(rank1[ii]);}
                else 
                    {alpha[ii]=float(rank2[ii]);}
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
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &high;
    vector<vector<float> > adv40;
    vector<vector<float> > hl;
    vector<vector<float> > hlv;
        int Num1; 
	int Num2;
	int Num3;
	int Num4;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha77(cfg);
        return str;
    }
}

